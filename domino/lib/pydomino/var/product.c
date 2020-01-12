
#include "db.h"
#include "util.h"
#include <datetime.h>
#include "product.h"

static void Product_dealloc(PyObject* self)
{
    do_free(((Product*)self)->priv);
    Py_DECREF(((Product*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("Product free\n");
}
static PyObject *Product_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("Product new\n");
    Product *self;
    self = PyObject_NEW(Product, type);
    //self = (Product *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(product_rec_t));
    return (PyObject *)self;
}
static int Product_init(Product *self, PyObject *args, PyObject *kwds)
{
    //LOG("Product init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    self->priv->size=sizeof(self->priv->data);

    do_text_set_empty(self->priv->data.class_id);

    do_text_set_empty(self->priv->data.group_id);

    do_text_set_empty(self->priv->data.subgroup_id);

    do_text_set_empty(self->priv->data.code);

    do_text_set_empty(self->priv->data.name);

    do_text_set_empty(self->priv->data.article);

    do_text_set_empty(self->priv->data.variant);

    do_text_set_empty(self->priv->data.unit);

    do_text_set_empty(self->priv->data.tech_unit);

    do_text_set_empty(self->priv->data.supplier_region);

    do_text_set_empty(self->priv->data.mfr_region);

    do_text_set_empty(self->priv->data.party);

    do_product_params_clear(self->alias->alias, self->priv);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *Product_clone(Product *self)
{
    Product *result;
    result = (Product*)PyObject_CallFunction((PyObject*)getProductType(),
                                    "O", self->alias);
    result->priv->size = self->priv->size;
    do_cpy(result->priv->data, self->priv->data);
    return (PyObject*)result;
}

static PyObject *Product_class_id(Product* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.class_id);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Product_set_class_id(Product* self, PyObject *args, PyObject *kwds)
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

static PyObject *Product_group_id(Product* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.group_id);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Product_set_group_id(Product* self, PyObject *args, PyObject *kwds)
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

static PyObject *Product_subgroup_id(Product* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.subgroup_id);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Product_set_subgroup_id(Product* self, PyObject *args, PyObject *kwds)
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

static PyObject *Product_code(Product* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Product_set_code(Product* self, PyObject *args, PyObject *kwds)
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

static PyObject *Product_name(Product* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.name);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Product_set_name(Product* self, PyObject *args, PyObject *kwds)
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

static PyObject *Product_article(Product* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.article);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Product_set_article(Product* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.article");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.article, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.article);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Product_variant(Product* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.variant);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Product_set_variant(Product* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.variant");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.variant, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.variant);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Product_unit(Product* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.unit);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Product_set_unit(Product* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.unit");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.unit, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.unit);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Product_tech_unit(Product* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.tech_unit);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Product_set_tech_unit(Product* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.tech_unit");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.tech_unit, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.tech_unit);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Product_supplier_region_code(Product* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.supplier_region);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Product_set_supplier_region_code(Product* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.supplier_region");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.supplier_region, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.supplier_region);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Product_supplier_code(Product* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.supplier_code);

    return result;
}

static PyObject *Product_set_supplier_code(Product* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.supplier_code");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.supplier_code = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.supplier_code = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.supplier_code");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.supplier_code);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Product_manufactor_region_code(Product* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.mfr_region);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Product_set_manufactor_region_code(Product* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.mfr_region");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.mfr_region, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.mfr_region);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Product_manufactor_code(Product* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.mfr_code);

    return result;
}

static PyObject *Product_set_manufactor_code(Product* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.mfr_code");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.mfr_code = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.mfr_code = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.mfr_code");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.mfr_code);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Product_product_type(Product* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.ptype);

    return result;
}

static PyObject *Product_set_product_type(Product* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.ptype");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.ptype = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.ptype = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.ptype");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.ptype);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Product_get_price(Product* self, PyObject *args, PyObject *kwds)
{
    PyObject *result;

    static char *kwlist[] = {"number", NULL};
    int number;
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &number) ) {
        do_log(LOG_ERR, "Invalid argument \"number\": wrong type. get price");
        return NULL;
    }
    double price;
    price = do_product_price(self->alias->alias, self->priv, number);
    result = PyFloat_FromDouble(price);

    return result;
}

static PyObject *Product_set_price(Product* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    int number;
    static char *kwlist[] = {"number", "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|O", kwlist, &number, &value) ) {
        do_log(LOG_ERR, "Invalid arguments \"number\" \"value\": wrong type. set price");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             do_product_price_set(self->alias->alias,self->priv, number, MyLong_AsLong(value));
        else
        if ( PyFloat_Check(value) )
             do_product_price_set(self->alias->alias,self->priv, number, PyFloat_AsDouble(value));
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set price");
            return NULL;
        }
    }
    /*double price;
    price = do_product_price(self->alias->alias, self->priv, number);
    result = PyFloat_FromDouble(price);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Product_coef(Product* self, void *unused)
{
    PyObject *result;

    result=PyFloat_FromDouble(do_pdecimal(self->priv->data.tech_unit_rate, 4, 3));

    return result;
}

static PyObject *Product_set_coef(Product* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.tech_unit_rate");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             do_pdecimal_set(self->priv->data.tech_unit_rate, 4, 3,  MyLong_AsLong(value));
        else
        if ( PyFloat_Check(value) )
             do_pdecimal_set(self->priv->data.tech_unit_rate, 4, 3,  PyFloat_AsDouble(value));
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.tech_unit_rate");
            return NULL;
        }    
    }
    //result=PyFloat_FromDouble(do_pdecimal(self->priv->data.tech_unit_rate, 4, 3));

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Product_date(Product* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_date(self->priv->data.date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *Product_date_raw(Product* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.date);

    return result;
}

static PyObject *Product_set_date(Product* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.date");
        return NULL;
    }
    struct tm tm;
    if ( value ) {
        if ( !PyDate_Check(value) && !PyDateTime_Check(value) && !MyLong_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.date");
            return NULL;
        }
        if ( MyLong_Check(value) ) {
            self->priv->data.date = MyLong_AsLong(value);
        } else {
            do_date_set_ymd(&tm, PyDateTime_GET_YEAR(value), PyDateTime_GET_MONTH(value), PyDateTime_GET_DAY(value));
            do_date_set(&self->priv->data.date, tm);
        }
    }
    /*do_date(self->priv->data.date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Product_time(Product* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_time(self->priv->data.time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);

    return result;
}

static PyObject *Product_time_raw(Product* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.time);

    return result;
}

static PyObject *Product_set_time(Product* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.time");
        return NULL;
    }
    struct tm tm;
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
             do_time_set(&self->priv->data.time, tm);
        }
    }
    /*do_time(self->priv->data.time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Product_party(Product* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.party);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Product_set_party(Product* self, PyObject *args, PyObject *kwds)
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

static PyObject *Product_is_parcel(Product* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.base_parcel);

    return result;
}

static PyObject *Product_set_is_parcel(Product* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.base_parcel");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.base_parcel = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.base_parcel = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.base_parcel");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.base_parcel);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Product_price_filled(Product* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.price_filled);

    return result;
}

static PyObject *Product_set_price_filled(Product* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.price_filled");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.price_filled = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.price_filled = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.price_filled");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.price_filled);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Product_total_price(Product* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.total_price);

    return result;
}

static PyObject *Product_set_total_price(Product* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.total_price");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.total_price = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.total_price = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.total_price");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.total_price);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Product_life_time(Product* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_date(self->priv->data.life_time, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *Product_set_life_time(Product* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.life_time");
        return NULL;
    }
    struct tm tm;
    if ( value ) {
        if ( !PyDate_Check(value) && !PyDateTime_Check(value) && !MyLong_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.life_time");
            return NULL;
        }
        if ( MyLong_Check(value) ) {
            self->priv->data.life_time = MyLong_AsLong(value);
        } else {
            do_date_set_ymd(&tm, PyDateTime_GET_YEAR(value), PyDateTime_GET_MONTH(value), PyDateTime_GET_DAY(value));
            do_date_set(&self->priv->data.life_time, tm);
        }
    }
    /*do_date(self->priv->data.life_time, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Product_set_param(Product* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    char *name;
    static char *kwlist[] = {"name", "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "sO|", kwlist, &name, &value) )
        return NULL;
    if ( MyString_Check(value) ) {
        char *val;
        val = MyString_AsString(value);
        do_product_param_set(self->alias->alias,self->priv, name, val);
        do_free(val);
    }
    else
    if ( MyLong_Check(value) )
        do_product_param_int_set(self->alias->alias,self->priv, name, MyLong_AsLong(value));
    else
    if ( PyFloat_Check(value) )
        do_product_param_float_set(self->alias->alias,self->priv, name, PyFloat_AsDouble(value));
    else {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *Product_set_params(Product* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    static char *kwlist[] = { "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &value) )
        return NULL;
    if ( PyDict_Check(value) ) {
        do_product_params_clear(self->alias->alias,self->priv);
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
                 do_product_param_set(self->alias->alias,self->priv, name, val);
                 do_free(val);
             }
             else
             if ( MyLong_Check(pValue) )
                 do_product_param_int_set(self->alias->alias,self->priv, name, MyLong_AsLong(pValue));
             else
             if ( PyFloat_Check(pValue) )
                 do_product_param_float_set(self->alias->alias,self->priv, name, PyFloat_AsDouble(pValue));
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
static PyObject *Product_get_param(Product* self, PyObject *args, PyObject *kwds)
{
    char *name;
    static char *kwlist[] = {"name", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "s|", kwlist, &name) )
        return NULL;
    char *buf;
    buf = do_product_param(self->alias->alias,self->priv, name);
    PyObject *result;
    result = MyString_FromString(buf);
    do_free(buf);
    return result;
}

static PyObject *Product_params_clear(Product* self)
{
    do_product_params_clear(self->alias->alias,self->priv);
    Py_INCREF(Py_None);
    return Py_None;
}
    
static PyObject *Product_get_params(Product* self, void *unused)
{   
    PyObject *res;
    char *params = do_alias_text_(self->alias->alias, self->priv->data.params, self->priv->size - (sizeof(self->priv->data) - sizeof(self->priv->data.params)));
   
    res = do_getparams(params);
    do_free(params);
    return res;
}


static PyObject *Product_prices_clear(Product* self)
{
    do_product_price_clear(self->alias->alias,self->priv);
    Py_INCREF(Py_None);
    return Py_None;
}
    
static PyObject *Product_get_prices(Product* self, void *unused)
{   
    PyObject *result;
    PyObject *val, *key;
    result = PyDict_New();
    int i;
    int number;
    double price;

    for ( i = 0; i < self->priv->data.total_price; i++ ) {
        number = self->priv->data.nprice[i];
        if ( !number ) break;
        price = do_product_price(self->alias->alias, self->priv, number);
        val = PyFloat_FromDouble(price);
        key = MyLong_FromLong(number);
        PyDict_SetItem(result, key, val);
    }
    return result;
}
static PyObject *Product_set_prices(Product* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    static char *kwlist[] = { "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &value) )
        return NULL;
    if ( PyDict_Check(value) ) {
        do_product_price_clear(self->alias->alias,self->priv);
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
                 do_product_price_set(self->alias->alias,self->priv, number, MyLong_AsLong(pValue));
             else
             if ( PyFloat_Check(pValue) )
                 do_product_price_set(self->alias->alias,self->priv, number, PyFloat_AsDouble(pValue));
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

static PyObject *Product_prev(Product* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProductKey0Type() )
        status = do_product_get0(self->alias->alias, self->priv, ((ProductKey0*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getProductKey1Type() )
        status = do_product_get1(self->alias->alias, self->priv, ((ProductKey1*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getProductKey2Type() )
        status = do_product_get2(self->alias->alias, self->priv, ((ProductKey2*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getProductKey3Type() )
        status = do_product_get3(self->alias->alias, self->priv, ((ProductKey3*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getProductKey4Type() )
        status = do_product_get4(self->alias->alias, self->priv, ((ProductKey4*)key)->priv, DO_GET_PREVIOUS);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Product_gt(Product* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProductKey0Type() )
        status = do_product_get0(self->alias->alias, self->priv, ((ProductKey0*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getProductKey1Type() )
        status = do_product_get1(self->alias->alias, self->priv, ((ProductKey1*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getProductKey2Type() )
        status = do_product_get2(self->alias->alias, self->priv, ((ProductKey2*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getProductKey3Type() )
        status = do_product_get3(self->alias->alias, self->priv, ((ProductKey3*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getProductKey4Type() )
        status = do_product_get4(self->alias->alias, self->priv, ((ProductKey4*)key)->priv, DO_GET_GT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Product_next(Product* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProductKey0Type() )
        status = do_product_get0(self->alias->alias, self->priv, ((ProductKey0*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getProductKey1Type() )
        status = do_product_get1(self->alias->alias, self->priv, ((ProductKey1*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getProductKey2Type() )
        status = do_product_get2(self->alias->alias, self->priv, ((ProductKey2*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getProductKey3Type() )
        status = do_product_get3(self->alias->alias, self->priv, ((ProductKey3*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getProductKey4Type() )
        status = do_product_get4(self->alias->alias, self->priv, ((ProductKey4*)key)->priv, DO_GET_NEXT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Product_ge(Product* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProductKey0Type() )
        status = do_product_get0(self->alias->alias, self->priv, ((ProductKey0*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getProductKey1Type() )
        status = do_product_get1(self->alias->alias, self->priv, ((ProductKey1*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getProductKey2Type() )
        status = do_product_get2(self->alias->alias, self->priv, ((ProductKey2*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getProductKey3Type() )
        status = do_product_get3(self->alias->alias, self->priv, ((ProductKey3*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getProductKey4Type() )
        status = do_product_get4(self->alias->alias, self->priv, ((ProductKey4*)key)->priv, DO_GET_GE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Product_equal(Product* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProductKey0Type() )
        status = do_product_get0(self->alias->alias, self->priv, ((ProductKey0*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getProductKey1Type() )
        status = do_product_get1(self->alias->alias, self->priv, ((ProductKey1*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getProductKey2Type() )
        status = do_product_get2(self->alias->alias, self->priv, ((ProductKey2*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getProductKey3Type() )
        status = do_product_get3(self->alias->alias, self->priv, ((ProductKey3*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getProductKey4Type() )
        status = do_product_get4(self->alias->alias, self->priv, ((ProductKey4*)key)->priv, DO_GET_EQUAL);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Product_last(Product* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProductKey0Type() )
        status = do_product_get0(self->alias->alias, self->priv, ((ProductKey0*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getProductKey1Type() )
        status = do_product_get1(self->alias->alias, self->priv, ((ProductKey1*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getProductKey2Type() )
        status = do_product_get2(self->alias->alias, self->priv, ((ProductKey2*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getProductKey3Type() )
        status = do_product_get3(self->alias->alias, self->priv, ((ProductKey3*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getProductKey4Type() )
        status = do_product_get4(self->alias->alias, self->priv, ((ProductKey4*)key)->priv, DO_GET_LAST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Product_lt(Product* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProductKey0Type() )
        status = do_product_get0(self->alias->alias, self->priv, ((ProductKey0*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getProductKey1Type() )
        status = do_product_get1(self->alias->alias, self->priv, ((ProductKey1*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getProductKey2Type() )
        status = do_product_get2(self->alias->alias, self->priv, ((ProductKey2*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getProductKey3Type() )
        status = do_product_get3(self->alias->alias, self->priv, ((ProductKey3*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getProductKey4Type() )
        status = do_product_get4(self->alias->alias, self->priv, ((ProductKey4*)key)->priv, DO_GET_LT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Product_le(Product* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProductKey0Type() )
        status = do_product_get0(self->alias->alias, self->priv, ((ProductKey0*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getProductKey1Type() )
        status = do_product_get1(self->alias->alias, self->priv, ((ProductKey1*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getProductKey2Type() )
        status = do_product_get2(self->alias->alias, self->priv, ((ProductKey2*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getProductKey3Type() )
        status = do_product_get3(self->alias->alias, self->priv, ((ProductKey3*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getProductKey4Type() )
        status = do_product_get4(self->alias->alias, self->priv, ((ProductKey4*)key)->priv, DO_GET_LE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Product_first(Product* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProductKey0Type() )
        status = do_product_get0(self->alias->alias, self->priv, ((ProductKey0*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getProductKey1Type() )
        status = do_product_get1(self->alias->alias, self->priv, ((ProductKey1*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getProductKey2Type() )
        status = do_product_get2(self->alias->alias, self->priv, ((ProductKey2*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getProductKey3Type() )
        status = do_product_get3(self->alias->alias, self->priv, ((ProductKey3*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getProductKey4Type() )
        status = do_product_get4(self->alias->alias, self->priv, ((ProductKey4*)key)->priv, DO_GET_FIRST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Product_iter_gt(Product* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProductKey0Type() ) {
        key_cmp = (product_key0_t*)do_malloc(sizeof(product_key0_t));
        memcpy(key_cmp, ((ProductKey0*)key)->priv, sizeof(product_key0_t));
        status = do_product_get0(self->alias->alias, self->priv, ((ProductKey0*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getProductKey1Type() ) {
        key_cmp = (product_key1_t*)do_malloc(sizeof(product_key1_t));
        memcpy(key_cmp, ((ProductKey1*)key)->priv, sizeof(product_key1_t));
        status = do_product_get1(self->alias->alias, self->priv, ((ProductKey1*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getProductKey2Type() ) {
        key_cmp = (product_key2_t*)do_malloc(sizeof(product_key2_t));
        memcpy(key_cmp, ((ProductKey2*)key)->priv, sizeof(product_key2_t));
        status = do_product_get2(self->alias->alias, self->priv, ((ProductKey2*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getProductKey3Type() ) {
        key_cmp = (product_key3_t*)do_malloc(sizeof(product_key3_t));
        memcpy(key_cmp, ((ProductKey3*)key)->priv, sizeof(product_key3_t));
        status = do_product_get3(self->alias->alias, self->priv, ((ProductKey3*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getProductKey4Type() ) {
        key_cmp = (product_key4_t*)do_malloc(sizeof(product_key4_t));
        memcpy(key_cmp, ((ProductKey4*)key)->priv, sizeof(product_key4_t));
        status = do_product_get4(self->alias->alias, self->priv, ((ProductKey4*)key)->priv, DO_GET_GT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProductKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_key0_t*)key_cmp)->base_parcel, 
                    ((ProductKey0*)key)->priv->base_parcel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_key0_t*)key_cmp)->code, 
                    ((ProductKey0*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProductKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_key1_t*)key_cmp)->base_parcel, 
                    ((ProductKey1*)key)->priv->base_parcel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_key1_t*)key_cmp)->class_id, 
                    ((ProductKey1*)key)->priv->class_id))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((product_key1_t*)key_cmp)->group_id, 
                    ((ProductKey1*)key)->priv->group_id))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((product_key1_t*)key_cmp)->subgroup_id, 
                    ((ProductKey1*)key)->priv->subgroup_id))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((product_key1_t*)key_cmp)->name, 
                    ((ProductKey1*)key)->priv->name))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProductKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_key2_t*)key_cmp)->base_parcel, 
                    ((ProductKey2*)key)->priv->base_parcel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_key2_t*)key_cmp)->article, 
                    ((ProductKey2*)key)->priv->article))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProductKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_key3_t*)key_cmp)->base_parcel, 
                    ((ProductKey3*)key)->priv->base_parcel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_key3_t*)key_cmp)->supplier_region, 
                    ((ProductKey3*)key)->priv->supplier_region))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((product_key3_t*)key_cmp)->supplier_code, 
                    ((ProductKey3*)key)->priv->supplier_code))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((product_key3_t*)key_cmp)->party, 
                    ((ProductKey3*)key)->priv->party))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProductKey4Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_key4_t*)key_cmp)->code, 
                    ((ProductKey4*)key)->priv->code))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Product_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProductKey0Type() ) {
            status = do_product_get0(self->alias->alias, self->priv, ((ProductKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProductKey1Type() ) {
            status = do_product_get1(self->alias->alias, self->priv, ((ProductKey1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProductKey2Type() ) {
            status = do_product_get2(self->alias->alias, self->priv, ((ProductKey2*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProductKey3Type() ) {
            status = do_product_get3(self->alias->alias, self->priv, ((ProductKey3*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProductKey4Type() ) {
            status = do_product_get4(self->alias->alias, self->priv, ((ProductKey4*)key)->priv, DO_GET_NEXT);
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

static PyObject *Product_iter_ge(Product* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProductKey0Type() ) {
        key_cmp = (product_key0_t*)do_malloc(sizeof(product_key0_t));
        memcpy(key_cmp, ((ProductKey0*)key)->priv, sizeof(product_key0_t));
        status = do_product_get0(self->alias->alias, self->priv, ((ProductKey0*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getProductKey1Type() ) {
        key_cmp = (product_key1_t*)do_malloc(sizeof(product_key1_t));
        memcpy(key_cmp, ((ProductKey1*)key)->priv, sizeof(product_key1_t));
        status = do_product_get1(self->alias->alias, self->priv, ((ProductKey1*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getProductKey2Type() ) {
        key_cmp = (product_key2_t*)do_malloc(sizeof(product_key2_t));
        memcpy(key_cmp, ((ProductKey2*)key)->priv, sizeof(product_key2_t));
        status = do_product_get2(self->alias->alias, self->priv, ((ProductKey2*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getProductKey3Type() ) {
        key_cmp = (product_key3_t*)do_malloc(sizeof(product_key3_t));
        memcpy(key_cmp, ((ProductKey3*)key)->priv, sizeof(product_key3_t));
        status = do_product_get3(self->alias->alias, self->priv, ((ProductKey3*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getProductKey4Type() ) {
        key_cmp = (product_key4_t*)do_malloc(sizeof(product_key4_t));
        memcpy(key_cmp, ((ProductKey4*)key)->priv, sizeof(product_key4_t));
        status = do_product_get4(self->alias->alias, self->priv, ((ProductKey4*)key)->priv, DO_GET_GE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProductKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_key0_t*)key_cmp)->base_parcel, 
                    ((ProductKey0*)key)->priv->base_parcel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_key0_t*)key_cmp)->code, 
                    ((ProductKey0*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProductKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_key1_t*)key_cmp)->base_parcel, 
                    ((ProductKey1*)key)->priv->base_parcel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_key1_t*)key_cmp)->class_id, 
                    ((ProductKey1*)key)->priv->class_id))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((product_key1_t*)key_cmp)->group_id, 
                    ((ProductKey1*)key)->priv->group_id))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((product_key1_t*)key_cmp)->subgroup_id, 
                    ((ProductKey1*)key)->priv->subgroup_id))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((product_key1_t*)key_cmp)->name, 
                    ((ProductKey1*)key)->priv->name))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProductKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_key2_t*)key_cmp)->base_parcel, 
                    ((ProductKey2*)key)->priv->base_parcel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_key2_t*)key_cmp)->article, 
                    ((ProductKey2*)key)->priv->article))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProductKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_key3_t*)key_cmp)->base_parcel, 
                    ((ProductKey3*)key)->priv->base_parcel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_key3_t*)key_cmp)->supplier_region, 
                    ((ProductKey3*)key)->priv->supplier_region))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((product_key3_t*)key_cmp)->supplier_code, 
                    ((ProductKey3*)key)->priv->supplier_code))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((product_key3_t*)key_cmp)->party, 
                    ((ProductKey3*)key)->priv->party))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProductKey4Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_key4_t*)key_cmp)->code, 
                    ((ProductKey4*)key)->priv->code))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Product_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProductKey0Type() ) {
            status = do_product_get0(self->alias->alias, self->priv, ((ProductKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProductKey1Type() ) {
            status = do_product_get1(self->alias->alias, self->priv, ((ProductKey1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProductKey2Type() ) {
            status = do_product_get2(self->alias->alias, self->priv, ((ProductKey2*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProductKey3Type() ) {
            status = do_product_get3(self->alias->alias, self->priv, ((ProductKey3*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProductKey4Type() ) {
            status = do_product_get4(self->alias->alias, self->priv, ((ProductKey4*)key)->priv, DO_GET_NEXT);
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

static PyObject *Product_iter_equal(Product* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProductKey0Type() ) {
        key_cmp = (product_key0_t*)do_malloc(sizeof(product_key0_t));
        memcpy(key_cmp, ((ProductKey0*)key)->priv, sizeof(product_key0_t));
        status = do_product_get0(self->alias->alias, self->priv, ((ProductKey0*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getProductKey1Type() ) {
        key_cmp = (product_key1_t*)do_malloc(sizeof(product_key1_t));
        memcpy(key_cmp, ((ProductKey1*)key)->priv, sizeof(product_key1_t));
        status = do_product_get1(self->alias->alias, self->priv, ((ProductKey1*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getProductKey2Type() ) {
        key_cmp = (product_key2_t*)do_malloc(sizeof(product_key2_t));
        memcpy(key_cmp, ((ProductKey2*)key)->priv, sizeof(product_key2_t));
        status = do_product_get2(self->alias->alias, self->priv, ((ProductKey2*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getProductKey3Type() ) {
        key_cmp = (product_key3_t*)do_malloc(sizeof(product_key3_t));
        memcpy(key_cmp, ((ProductKey3*)key)->priv, sizeof(product_key3_t));
        status = do_product_get3(self->alias->alias, self->priv, ((ProductKey3*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getProductKey4Type() ) {
        key_cmp = (product_key4_t*)do_malloc(sizeof(product_key4_t));
        memcpy(key_cmp, ((ProductKey4*)key)->priv, sizeof(product_key4_t));
        status = do_product_get4(self->alias->alias, self->priv, ((ProductKey4*)key)->priv, DO_GET_EQUAL);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProductKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_key0_t*)key_cmp)->base_parcel, 
                    ((ProductKey0*)key)->priv->base_parcel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_key0_t*)key_cmp)->code, 
                    ((ProductKey0*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProductKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_key1_t*)key_cmp)->base_parcel, 
                    ((ProductKey1*)key)->priv->base_parcel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_key1_t*)key_cmp)->class_id, 
                    ((ProductKey1*)key)->priv->class_id))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((product_key1_t*)key_cmp)->group_id, 
                    ((ProductKey1*)key)->priv->group_id))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((product_key1_t*)key_cmp)->subgroup_id, 
                    ((ProductKey1*)key)->priv->subgroup_id))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((product_key1_t*)key_cmp)->name, 
                    ((ProductKey1*)key)->priv->name))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProductKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_key2_t*)key_cmp)->base_parcel, 
                    ((ProductKey2*)key)->priv->base_parcel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_key2_t*)key_cmp)->article, 
                    ((ProductKey2*)key)->priv->article))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProductKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_key3_t*)key_cmp)->base_parcel, 
                    ((ProductKey3*)key)->priv->base_parcel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_key3_t*)key_cmp)->supplier_region, 
                    ((ProductKey3*)key)->priv->supplier_region))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((product_key3_t*)key_cmp)->supplier_code, 
                    ((ProductKey3*)key)->priv->supplier_code))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((product_key3_t*)key_cmp)->party, 
                    ((ProductKey3*)key)->priv->party))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProductKey4Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_key4_t*)key_cmp)->code, 
                    ((ProductKey4*)key)->priv->code))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Product_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProductKey0Type() ) {
            status = do_product_get0(self->alias->alias, self->priv, ((ProductKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProductKey1Type() ) {
            status = do_product_get1(self->alias->alias, self->priv, ((ProductKey1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProductKey2Type() ) {
            status = do_product_get2(self->alias->alias, self->priv, ((ProductKey2*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProductKey3Type() ) {
            status = do_product_get3(self->alias->alias, self->priv, ((ProductKey3*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProductKey4Type() ) {
            status = do_product_get4(self->alias->alias, self->priv, ((ProductKey4*)key)->priv, DO_GET_NEXT);
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

static PyObject *Product_iter_last(Product* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProductKey0Type() ) {
        key_cmp = (product_key0_t*)do_malloc(sizeof(product_key0_t));
        memcpy(key_cmp, ((ProductKey0*)key)->priv, sizeof(product_key0_t));
        status = do_product_get0(self->alias->alias, self->priv, ((ProductKey0*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getProductKey1Type() ) {
        key_cmp = (product_key1_t*)do_malloc(sizeof(product_key1_t));
        memcpy(key_cmp, ((ProductKey1*)key)->priv, sizeof(product_key1_t));
        status = do_product_get1(self->alias->alias, self->priv, ((ProductKey1*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getProductKey2Type() ) {
        key_cmp = (product_key2_t*)do_malloc(sizeof(product_key2_t));
        memcpy(key_cmp, ((ProductKey2*)key)->priv, sizeof(product_key2_t));
        status = do_product_get2(self->alias->alias, self->priv, ((ProductKey2*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getProductKey3Type() ) {
        key_cmp = (product_key3_t*)do_malloc(sizeof(product_key3_t));
        memcpy(key_cmp, ((ProductKey3*)key)->priv, sizeof(product_key3_t));
        status = do_product_get3(self->alias->alias, self->priv, ((ProductKey3*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getProductKey4Type() ) {
        key_cmp = (product_key4_t*)do_malloc(sizeof(product_key4_t));
        memcpy(key_cmp, ((ProductKey4*)key)->priv, sizeof(product_key4_t));
        status = do_product_get4(self->alias->alias, self->priv, ((ProductKey4*)key)->priv, DO_GET_LAST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProductKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_key0_t*)key_cmp)->base_parcel, 
                    ((ProductKey0*)key)->priv->base_parcel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_key0_t*)key_cmp)->code, 
                    ((ProductKey0*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProductKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_key1_t*)key_cmp)->base_parcel, 
                    ((ProductKey1*)key)->priv->base_parcel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_key1_t*)key_cmp)->class_id, 
                    ((ProductKey1*)key)->priv->class_id))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((product_key1_t*)key_cmp)->group_id, 
                    ((ProductKey1*)key)->priv->group_id))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((product_key1_t*)key_cmp)->subgroup_id, 
                    ((ProductKey1*)key)->priv->subgroup_id))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((product_key1_t*)key_cmp)->name, 
                    ((ProductKey1*)key)->priv->name))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProductKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_key2_t*)key_cmp)->base_parcel, 
                    ((ProductKey2*)key)->priv->base_parcel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_key2_t*)key_cmp)->article, 
                    ((ProductKey2*)key)->priv->article))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProductKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_key3_t*)key_cmp)->base_parcel, 
                    ((ProductKey3*)key)->priv->base_parcel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_key3_t*)key_cmp)->supplier_region, 
                    ((ProductKey3*)key)->priv->supplier_region))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((product_key3_t*)key_cmp)->supplier_code, 
                    ((ProductKey3*)key)->priv->supplier_code))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((product_key3_t*)key_cmp)->party, 
                    ((ProductKey3*)key)->priv->party))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProductKey4Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_key4_t*)key_cmp)->code, 
                    ((ProductKey4*)key)->priv->code))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Product_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProductKey0Type() ) {
            status = do_product_get0(self->alias->alias, self->priv, ((ProductKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProductKey1Type() ) {
            status = do_product_get1(self->alias->alias, self->priv, ((ProductKey1*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProductKey2Type() ) {
            status = do_product_get2(self->alias->alias, self->priv, ((ProductKey2*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProductKey3Type() ) {
            status = do_product_get3(self->alias->alias, self->priv, ((ProductKey3*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProductKey4Type() ) {
            status = do_product_get4(self->alias->alias, self->priv, ((ProductKey4*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Product_iter_lt(Product* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProductKey0Type() ) {
        key_cmp = (product_key0_t*)do_malloc(sizeof(product_key0_t));
        memcpy(key_cmp, ((ProductKey0*)key)->priv, sizeof(product_key0_t));
        status = do_product_get0(self->alias->alias, self->priv, ((ProductKey0*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getProductKey1Type() ) {
        key_cmp = (product_key1_t*)do_malloc(sizeof(product_key1_t));
        memcpy(key_cmp, ((ProductKey1*)key)->priv, sizeof(product_key1_t));
        status = do_product_get1(self->alias->alias, self->priv, ((ProductKey1*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getProductKey2Type() ) {
        key_cmp = (product_key2_t*)do_malloc(sizeof(product_key2_t));
        memcpy(key_cmp, ((ProductKey2*)key)->priv, sizeof(product_key2_t));
        status = do_product_get2(self->alias->alias, self->priv, ((ProductKey2*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getProductKey3Type() ) {
        key_cmp = (product_key3_t*)do_malloc(sizeof(product_key3_t));
        memcpy(key_cmp, ((ProductKey3*)key)->priv, sizeof(product_key3_t));
        status = do_product_get3(self->alias->alias, self->priv, ((ProductKey3*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getProductKey4Type() ) {
        key_cmp = (product_key4_t*)do_malloc(sizeof(product_key4_t));
        memcpy(key_cmp, ((ProductKey4*)key)->priv, sizeof(product_key4_t));
        status = do_product_get4(self->alias->alias, self->priv, ((ProductKey4*)key)->priv, DO_GET_LT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProductKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_key0_t*)key_cmp)->base_parcel, 
                    ((ProductKey0*)key)->priv->base_parcel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_key0_t*)key_cmp)->code, 
                    ((ProductKey0*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProductKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_key1_t*)key_cmp)->base_parcel, 
                    ((ProductKey1*)key)->priv->base_parcel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_key1_t*)key_cmp)->class_id, 
                    ((ProductKey1*)key)->priv->class_id))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((product_key1_t*)key_cmp)->group_id, 
                    ((ProductKey1*)key)->priv->group_id))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((product_key1_t*)key_cmp)->subgroup_id, 
                    ((ProductKey1*)key)->priv->subgroup_id))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((product_key1_t*)key_cmp)->name, 
                    ((ProductKey1*)key)->priv->name))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProductKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_key2_t*)key_cmp)->base_parcel, 
                    ((ProductKey2*)key)->priv->base_parcel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_key2_t*)key_cmp)->article, 
                    ((ProductKey2*)key)->priv->article))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProductKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_key3_t*)key_cmp)->base_parcel, 
                    ((ProductKey3*)key)->priv->base_parcel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_key3_t*)key_cmp)->supplier_region, 
                    ((ProductKey3*)key)->priv->supplier_region))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((product_key3_t*)key_cmp)->supplier_code, 
                    ((ProductKey3*)key)->priv->supplier_code))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((product_key3_t*)key_cmp)->party, 
                    ((ProductKey3*)key)->priv->party))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProductKey4Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_key4_t*)key_cmp)->code, 
                    ((ProductKey4*)key)->priv->code))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Product_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProductKey0Type() ) {
            status = do_product_get0(self->alias->alias, self->priv, ((ProductKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProductKey1Type() ) {
            status = do_product_get1(self->alias->alias, self->priv, ((ProductKey1*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProductKey2Type() ) {
            status = do_product_get2(self->alias->alias, self->priv, ((ProductKey2*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProductKey3Type() ) {
            status = do_product_get3(self->alias->alias, self->priv, ((ProductKey3*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProductKey4Type() ) {
            status = do_product_get4(self->alias->alias, self->priv, ((ProductKey4*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Product_iter_le(Product* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProductKey0Type() ) {
        key_cmp = (product_key0_t*)do_malloc(sizeof(product_key0_t));
        memcpy(key_cmp, ((ProductKey0*)key)->priv, sizeof(product_key0_t));
        status = do_product_get0(self->alias->alias, self->priv, ((ProductKey0*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getProductKey1Type() ) {
        key_cmp = (product_key1_t*)do_malloc(sizeof(product_key1_t));
        memcpy(key_cmp, ((ProductKey1*)key)->priv, sizeof(product_key1_t));
        status = do_product_get1(self->alias->alias, self->priv, ((ProductKey1*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getProductKey2Type() ) {
        key_cmp = (product_key2_t*)do_malloc(sizeof(product_key2_t));
        memcpy(key_cmp, ((ProductKey2*)key)->priv, sizeof(product_key2_t));
        status = do_product_get2(self->alias->alias, self->priv, ((ProductKey2*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getProductKey3Type() ) {
        key_cmp = (product_key3_t*)do_malloc(sizeof(product_key3_t));
        memcpy(key_cmp, ((ProductKey3*)key)->priv, sizeof(product_key3_t));
        status = do_product_get3(self->alias->alias, self->priv, ((ProductKey3*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getProductKey4Type() ) {
        key_cmp = (product_key4_t*)do_malloc(sizeof(product_key4_t));
        memcpy(key_cmp, ((ProductKey4*)key)->priv, sizeof(product_key4_t));
        status = do_product_get4(self->alias->alias, self->priv, ((ProductKey4*)key)->priv, DO_GET_LE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProductKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_key0_t*)key_cmp)->base_parcel, 
                    ((ProductKey0*)key)->priv->base_parcel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_key0_t*)key_cmp)->code, 
                    ((ProductKey0*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProductKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_key1_t*)key_cmp)->base_parcel, 
                    ((ProductKey1*)key)->priv->base_parcel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_key1_t*)key_cmp)->class_id, 
                    ((ProductKey1*)key)->priv->class_id))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((product_key1_t*)key_cmp)->group_id, 
                    ((ProductKey1*)key)->priv->group_id))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((product_key1_t*)key_cmp)->subgroup_id, 
                    ((ProductKey1*)key)->priv->subgroup_id))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((product_key1_t*)key_cmp)->name, 
                    ((ProductKey1*)key)->priv->name))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProductKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_key2_t*)key_cmp)->base_parcel, 
                    ((ProductKey2*)key)->priv->base_parcel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_key2_t*)key_cmp)->article, 
                    ((ProductKey2*)key)->priv->article))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProductKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_key3_t*)key_cmp)->base_parcel, 
                    ((ProductKey3*)key)->priv->base_parcel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_key3_t*)key_cmp)->supplier_region, 
                    ((ProductKey3*)key)->priv->supplier_region))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((product_key3_t*)key_cmp)->supplier_code, 
                    ((ProductKey3*)key)->priv->supplier_code))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((product_key3_t*)key_cmp)->party, 
                    ((ProductKey3*)key)->priv->party))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProductKey4Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_key4_t*)key_cmp)->code, 
                    ((ProductKey4*)key)->priv->code))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Product_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProductKey0Type() ) {
            status = do_product_get0(self->alias->alias, self->priv, ((ProductKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProductKey1Type() ) {
            status = do_product_get1(self->alias->alias, self->priv, ((ProductKey1*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProductKey2Type() ) {
            status = do_product_get2(self->alias->alias, self->priv, ((ProductKey2*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProductKey3Type() ) {
            status = do_product_get3(self->alias->alias, self->priv, ((ProductKey3*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProductKey4Type() ) {
            status = do_product_get4(self->alias->alias, self->priv, ((ProductKey4*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Product_iter_first(Product* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProductKey0Type() ) {
        key_cmp = (product_key0_t*)do_malloc(sizeof(product_key0_t));
        memcpy(key_cmp, ((ProductKey0*)key)->priv, sizeof(product_key0_t));
        status = do_product_get0(self->alias->alias, self->priv, ((ProductKey0*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getProductKey1Type() ) {
        key_cmp = (product_key1_t*)do_malloc(sizeof(product_key1_t));
        memcpy(key_cmp, ((ProductKey1*)key)->priv, sizeof(product_key1_t));
        status = do_product_get1(self->alias->alias, self->priv, ((ProductKey1*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getProductKey2Type() ) {
        key_cmp = (product_key2_t*)do_malloc(sizeof(product_key2_t));
        memcpy(key_cmp, ((ProductKey2*)key)->priv, sizeof(product_key2_t));
        status = do_product_get2(self->alias->alias, self->priv, ((ProductKey2*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getProductKey3Type() ) {
        key_cmp = (product_key3_t*)do_malloc(sizeof(product_key3_t));
        memcpy(key_cmp, ((ProductKey3*)key)->priv, sizeof(product_key3_t));
        status = do_product_get3(self->alias->alias, self->priv, ((ProductKey3*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getProductKey4Type() ) {
        key_cmp = (product_key4_t*)do_malloc(sizeof(product_key4_t));
        memcpy(key_cmp, ((ProductKey4*)key)->priv, sizeof(product_key4_t));
        status = do_product_get4(self->alias->alias, self->priv, ((ProductKey4*)key)->priv, DO_GET_FIRST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProductKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_key0_t*)key_cmp)->base_parcel, 
                    ((ProductKey0*)key)->priv->base_parcel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_key0_t*)key_cmp)->code, 
                    ((ProductKey0*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProductKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_key1_t*)key_cmp)->base_parcel, 
                    ((ProductKey1*)key)->priv->base_parcel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_key1_t*)key_cmp)->class_id, 
                    ((ProductKey1*)key)->priv->class_id))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((product_key1_t*)key_cmp)->group_id, 
                    ((ProductKey1*)key)->priv->group_id))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((product_key1_t*)key_cmp)->subgroup_id, 
                    ((ProductKey1*)key)->priv->subgroup_id))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((product_key1_t*)key_cmp)->name, 
                    ((ProductKey1*)key)->priv->name))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProductKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_key2_t*)key_cmp)->base_parcel, 
                    ((ProductKey2*)key)->priv->base_parcel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_key2_t*)key_cmp)->article, 
                    ((ProductKey2*)key)->priv->article))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProductKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_key3_t*)key_cmp)->base_parcel, 
                    ((ProductKey3*)key)->priv->base_parcel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_key3_t*)key_cmp)->supplier_region, 
                    ((ProductKey3*)key)->priv->supplier_region))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((product_key3_t*)key_cmp)->supplier_code, 
                    ((ProductKey3*)key)->priv->supplier_code))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((product_key3_t*)key_cmp)->party, 
                    ((ProductKey3*)key)->priv->party))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProductKey4Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_key4_t*)key_cmp)->code, 
                    ((ProductKey4*)key)->priv->code))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Product_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProductKey0Type() ) {
            status = do_product_get0(self->alias->alias, self->priv, ((ProductKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProductKey1Type() ) {
            status = do_product_get1(self->alias->alias, self->priv, ((ProductKey1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProductKey2Type() ) {
            status = do_product_get2(self->alias->alias, self->priv, ((ProductKey2*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProductKey3Type() ) {
            status = do_product_get3(self->alias->alias, self->priv, ((ProductKey3*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProductKey4Type() ) {
            status = do_product_get4(self->alias->alias, self->priv, ((ProductKey4*)key)->priv, DO_GET_NEXT);
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

static PyObject *Product_update(Product* self)
{
    int status;
    status = do_product_update(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Product_insert(Product* self)
{
    int status;
    status = do_product_insert(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Product_delete(Product* self)
{
    int status;
    status = do_product_delete(self->alias->alias);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Product_set_alias(Product* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((Product*)self)->alias);
    Py_INCREF(obj);
    ((Product*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Product_set(Product* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getProductType() ) {
        memcpy(self->priv, ((Product*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getProductKey0Type() ) {

        do_cpy(self->priv->data.code, ((ProductKey0*)obj)->priv->code);

        do_cpy(self->priv->data.base_parcel, ((ProductKey0*)obj)->priv->base_parcel);

    }

    else 
    if ( Py_TYPE(obj) == getProductKey1Type() ) {

        do_cpy(self->priv->data.class_id, ((ProductKey1*)obj)->priv->class_id);

        do_cpy(self->priv->data.group_id, ((ProductKey1*)obj)->priv->group_id);

        do_cpy(self->priv->data.subgroup_id, ((ProductKey1*)obj)->priv->subgroup_id);

        do_cpy(self->priv->data.name, ((ProductKey1*)obj)->priv->name);

        do_cpy(self->priv->data.base_parcel, ((ProductKey1*)obj)->priv->base_parcel);

    }

    else 
    if ( Py_TYPE(obj) == getProductKey2Type() ) {

        do_cpy(self->priv->data.article, ((ProductKey2*)obj)->priv->article);

        do_cpy(self->priv->data.base_parcel, ((ProductKey2*)obj)->priv->base_parcel);

    }

    else 
    if ( Py_TYPE(obj) == getProductKey3Type() ) {

        do_cpy(self->priv->data.supplier_region, ((ProductKey3*)obj)->priv->supplier_region);

        do_cpy(self->priv->data.supplier_code, ((ProductKey3*)obj)->priv->supplier_code);

        do_cpy(self->priv->data.party, ((ProductKey3*)obj)->priv->party);

        do_cpy(self->priv->data.base_parcel, ((ProductKey3*)obj)->priv->base_parcel);

    }

    else 
    if ( Py_TYPE(obj) == getProductKey4Type() ) {

        do_cpy(self->priv->data.code, ((ProductKey4*)obj)->priv->code);

    }

    else 
    if ( Py_TYPE(obj) == getProductDataType() ) {

        do_cpy(self->priv->data.code, ((ProductData*)obj)->priv->data.code);

    }

    else 
    if ( Py_TYPE(obj) == getProductDataKey0Type() ) {

        do_cpy(self->priv->data.code, ((ProductDataKey0*)obj)->priv->code);

    }

    else 
    if ( Py_TYPE(obj) == getProductViewType() ) {

        do_cpy(self->priv->data.code, ((ProductView*)obj)->priv->data.code);

    }

    else 
    if ( Py_TYPE(obj) == getProductViewKey0Type() ) {

        do_cpy(self->priv->data.code, ((ProductViewKey0*)obj)->priv->code);

    }

    else 
    if ( Py_TYPE(obj) == getProductViewKey1Type() ) {

        do_cpy(self->priv->data.code, ((ProductViewKey1*)obj)->priv->code);

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

            else
            if ( !strcmp("code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("name", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_name", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("article", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_article", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("variant", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_variant", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("unit", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_unit", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("tech_unit", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_tech_unit", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("supplier_region_code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_supplier_region_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("supplier_code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_supplier_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("manufactor_region_code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_manufactor_region_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("manufactor_code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_manufactor_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("product_type", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_product_type", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("price", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_price", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("prices", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_prices", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("coef", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_coef", "O", value) )
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
            if ( !strcmp("party", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_party", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("is_parcel", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_is_parcel", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("price_filled", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_price_filled", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("total_price", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_total_price", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("life_time", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_life_time", "O", value) )
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
                do_log(LOG_ERR, "Class \"Product\"does not contain \"%s\"", field_name);
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

static PyObject *Product_fields(Product* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class Product");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  Product_class_id(self,NULL);
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

    value =  Product_group_id(self,NULL);
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

    value =  Product_subgroup_id(self,NULL);
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

    value =  Product_code(self,NULL);
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

    value =  Product_name(self,NULL);
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

    value =  Product_article(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("article", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Product_variant(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("variant", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Product_unit(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("unit", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Product_tech_unit(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("tech_unit", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Product_supplier_region_code(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("supplier_region_code", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Product_supplier_code(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("supplier_code", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Product_manufactor_region_code(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("manufactor_region_code", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Product_manufactor_code(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("manufactor_code", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Product_product_type(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("product_type", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value = Product_get_prices(self, NULL);
    buf=get_prices_desc(value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    buf = do_strdup_printf("Coef:%f", do_product_coef(self->alias->alias, self->priv));
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Product_date(self,NULL);
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

    value =  Product_time(self,NULL);
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

    value =  Product_party(self,NULL);
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

    value =  Product_is_parcel(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("is_parcel", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Product_price_filled(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("price_filled", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Product_total_price(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("total_price", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Product_life_time(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("life_time", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value = Product_get_params(self, NULL);
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

static PyGetSetDef Product_getset[] = {

    {"class_id",(getter)Product_class_id},

    {"group_id",(getter)Product_group_id},

    {"subgroup_id",(getter)Product_subgroup_id},

    {"code",(getter)Product_code},

    {"name",(getter)Product_name},

    {"article",(getter)Product_article},

    {"variant",(getter)Product_variant},

    {"unit",(getter)Product_unit},

    {"tech_unit",(getter)Product_tech_unit},

    {"supplier_region_code",(getter)Product_supplier_region_code},

    {"supplier_code",(getter)Product_supplier_code},

    {"manufactor_region_code",(getter)Product_manufactor_region_code},

    {"manufactor_code",(getter)Product_manufactor_code},

    {"product_type",(getter)Product_product_type},

    {"prices",(getter)Product_get_prices},

    {"coef",(getter)Product_coef},

    {"date",(getter)Product_date},

    {"date_raw",(getter)Product_date_raw},

    {"time",(getter)Product_time},

    {"time_raw",(getter)Product_time_raw},

    {"party",(getter)Product_party},

    {"is_parcel",(getter)Product_is_parcel},

    {"price_filled",(getter)Product_price_filled},

    {"total_price",(getter)Product_total_price},

    {"life_time",(getter)Product_life_time},

    {"params",(getter)Product_get_params},

    {NULL}
};

static PyMethodDef Product_methods[] = {
    {"set", (PyCFunction)Product_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)Product_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)Product_clone, METH_NOARGS, "clone"},

    {"clear_prices", (PyCFunction)Product_prices_clear, METH_NOARGS, "do_Product_prices_clear"},
    {"price", (PyCFunction)Product_get_price, METH_VARARGS|METH_KEYWORDS, "do_Product_price"},
    {"set_prices", (PyCFunction)Product_set_prices, METH_VARARGS|METH_KEYWORDS, "do_Product_set_prices"},

    {"set_class_id", (PyCFunction)Product_set_class_id, METH_VARARGS|METH_KEYWORDS, "Product_set_class_id"},

    {"set_group_id", (PyCFunction)Product_set_group_id, METH_VARARGS|METH_KEYWORDS, "Product_set_group_id"},

    {"set_subgroup_id", (PyCFunction)Product_set_subgroup_id, METH_VARARGS|METH_KEYWORDS, "Product_set_subgroup_id"},

    {"set_code", (PyCFunction)Product_set_code, METH_VARARGS|METH_KEYWORDS, "Product_set_code"},

    {"set_name", (PyCFunction)Product_set_name, METH_VARARGS|METH_KEYWORDS, "Product_set_name"},

    {"set_article", (PyCFunction)Product_set_article, METH_VARARGS|METH_KEYWORDS, "Product_set_article"},

    {"set_variant", (PyCFunction)Product_set_variant, METH_VARARGS|METH_KEYWORDS, "Product_set_variant"},

    {"set_unit", (PyCFunction)Product_set_unit, METH_VARARGS|METH_KEYWORDS, "Product_set_unit"},

    {"set_tech_unit", (PyCFunction)Product_set_tech_unit, METH_VARARGS|METH_KEYWORDS, "Product_set_tech_unit"},

    {"set_supplier_region_code", (PyCFunction)Product_set_supplier_region_code, METH_VARARGS|METH_KEYWORDS, "Product_set_supplier_region_code"},

    {"set_supplier_code", (PyCFunction)Product_set_supplier_code, METH_VARARGS|METH_KEYWORDS, "Product_set_supplier_code"},

    {"set_manufactor_region_code", (PyCFunction)Product_set_manufactor_region_code, METH_VARARGS|METH_KEYWORDS, "Product_set_manufactor_region_code"},

    {"set_manufactor_code", (PyCFunction)Product_set_manufactor_code, METH_VARARGS|METH_KEYWORDS, "Product_set_manufactor_code"},

    {"set_product_type", (PyCFunction)Product_set_product_type, METH_VARARGS|METH_KEYWORDS, "Product_set_product_type"},

    {"set_price", (PyCFunction)Product_set_price, METH_VARARGS|METH_KEYWORDS, "Product_set_price"},

    {"set_coef", (PyCFunction)Product_set_coef, METH_VARARGS|METH_KEYWORDS, "Product_set_coef"},

    {"set_date", (PyCFunction)Product_set_date, METH_VARARGS|METH_KEYWORDS, "Product_set_date"},

    {"set_time", (PyCFunction)Product_set_time, METH_VARARGS|METH_KEYWORDS, "Product_set_time"},

    {"set_party", (PyCFunction)Product_set_party, METH_VARARGS|METH_KEYWORDS, "Product_set_party"},

    {"set_is_parcel", (PyCFunction)Product_set_is_parcel, METH_VARARGS|METH_KEYWORDS, "Product_set_is_parcel"},

    {"set_price_filled", (PyCFunction)Product_set_price_filled, METH_VARARGS|METH_KEYWORDS, "Product_set_price_filled"},

    {"set_total_price", (PyCFunction)Product_set_total_price, METH_VARARGS|METH_KEYWORDS, "Product_set_total_price"},

    {"set_life_time", (PyCFunction)Product_set_life_time, METH_VARARGS|METH_KEYWORDS, "Product_set_life_time"},

    {"set_param", (PyCFunction)Product_set_param, METH_VARARGS|METH_KEYWORDS, "do_Product_param_set"},
    {"param", (PyCFunction)Product_get_param, METH_VARARGS|METH_KEYWORDS, "do_Product_param"},
    {"clear_params", (PyCFunction)Product_params_clear, METH_NOARGS, "do_Product_param_clear"},
    {"set_params", (PyCFunction)Product_set_params, METH_VARARGS|METH_KEYWORDS, "do_Product_set_params"},

    {"get_prev", (PyCFunction)Product_prev, METH_VARARGS|METH_KEYWORDS, "Product_prev"},

    {"get_gt", (PyCFunction)Product_gt, METH_VARARGS|METH_KEYWORDS, "Product_gt"},

    {"get_next", (PyCFunction)Product_next, METH_VARARGS|METH_KEYWORDS, "Product_next"},

    {"get_ge", (PyCFunction)Product_ge, METH_VARARGS|METH_KEYWORDS, "Product_ge"},

    {"get_equal", (PyCFunction)Product_equal, METH_VARARGS|METH_KEYWORDS, "Product_equal"},

    {"get_last", (PyCFunction)Product_last, METH_VARARGS|METH_KEYWORDS, "Product_last"},

    {"get_lt", (PyCFunction)Product_lt, METH_VARARGS|METH_KEYWORDS, "Product_lt"},

    {"get_le", (PyCFunction)Product_le, METH_VARARGS|METH_KEYWORDS, "Product_le"},

    {"get_first", (PyCFunction)Product_first, METH_VARARGS|METH_KEYWORDS, "Product_first"},

    {"gets_gt", (PyCFunction)Product_iter_gt, METH_VARARGS|METH_KEYWORDS, "Product_iter_gt"},

    {"gets_ge", (PyCFunction)Product_iter_ge, METH_VARARGS|METH_KEYWORDS, "Product_iter_ge"},

    {"gets_equal", (PyCFunction)Product_iter_equal, METH_VARARGS|METH_KEYWORDS, "Product_iter_equal"},

    {"gets_last", (PyCFunction)Product_iter_last, METH_VARARGS|METH_KEYWORDS, "Product_iter_last"},

    {"gets_lt", (PyCFunction)Product_iter_lt, METH_VARARGS|METH_KEYWORDS, "Product_iter_lt"},

    {"gets_le", (PyCFunction)Product_iter_le, METH_VARARGS|METH_KEYWORDS, "Product_iter_le"},

    {"gets_first", (PyCFunction)Product_iter_first, METH_VARARGS|METH_KEYWORDS, "Product_iter_first"},

    {"update", (PyCFunction)Product_update, METH_VARARGS|METH_KEYWORDS, "Product_update"},

    {"insert", (PyCFunction)Product_insert, METH_VARARGS|METH_KEYWORDS, "Product_insert"},

    {"delete", (PyCFunction)Product_delete, METH_VARARGS|METH_KEYWORDS, "Product_delete"},

    {NULL}
};

static PyTypeObject ProductType_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.Product",             /*tp_name*/
    sizeof(Product),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Product_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)Product_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Product objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Product_methods,             /* tp_methods */
    0,
    Product_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Product_init,      /* tp_init */
    0,                         /* tp_alloc */
    Product_new,                 /* tp_new */
};
PyTypeObject *getProductType()
{
    return &ProductType_;
}

static void ProductKey0_dealloc(PyObject* self)
{
    do_free(((ProductKey0*)self)->priv);
    Py_DECREF(((ProductKey0*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("ProductKey0 free\n");
}
static PyObject *ProductKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("ProductKey0 new\n");
    ProductKey0 *self;
    self = PyObject_NEW(ProductKey0, type);
    //self = (ProductKey0 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(product_key0_t));
    return (PyObject *)self;
}
static int ProductKey0_init(ProductKey0 *self, PyObject *args, PyObject *kwds)
{
    //LOG("ProductKey0 init\n");
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

static PyObject *ProductKey0_clone(ProductKey0 *self)
{
    ProductKey0 *result;
    result = (ProductKey0*)PyObject_CallFunction((PyObject*)getProductKey0Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *ProductKey0_is_parcel(ProductKey0* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->base_parcel);

    return result;
}

static PyObject *ProductKey0_set_is_parcel(ProductKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set base_parcel");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->base_parcel = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->base_parcel = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set base_parcel");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->base_parcel);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *ProductKey0_code(ProductKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ProductKey0_set_code(ProductKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *ProductKey0_prev(ProductKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey0_gt(ProductKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key0(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey0_next(ProductKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key0(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey0_ge(ProductKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key0(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey0_equal(ProductKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key0(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey0_last(ProductKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key0(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey0_lt(ProductKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key0(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey0_le(ProductKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key0(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey0_first(ProductKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key0(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey0_iter_gt(ProductKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_key0(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.base_parcel, 
                 self->priv->base_parcel))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = ProductKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductKey0_iter_ge(ProductKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_key0(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.base_parcel, 
                 self->priv->base_parcel))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = ProductKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductKey0_iter_equal(ProductKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_key0(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.base_parcel, 
                 self->priv->base_parcel))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = ProductKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductKey0_iter_last(ProductKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_key0(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.base_parcel, 
                 self->priv->base_parcel))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = ProductKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductKey0_iter_lt(ProductKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_key0(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.base_parcel, 
                 self->priv->base_parcel))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = ProductKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductKey0_iter_le(ProductKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_key0(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.base_parcel, 
                 self->priv->base_parcel))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = ProductKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductKey0_iter_first(ProductKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_key0(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.base_parcel, 
                 self->priv->base_parcel))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = ProductKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductKey0_cmp(ProductKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getProductKey0Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = self->priv->base_parcel - ((ProductKey0*)obj)->priv->base_parcel;
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->code, ((ProductKey0*)obj)->priv->code);
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *ProductKey0_set_alias(ProductKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((ProductKey0*)self)->alias);
    Py_INCREF(obj);
    ((ProductKey0*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *ProductKey0_set(ProductKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getProductKey0Type() ) {
        memcpy(self->priv, ((ProductKey0*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getProductType() ) {

        do_cpy(self->priv->base_parcel, ((Product*)obj)->priv->data.base_parcel);

        do_cpy(self->priv->code, ((Product*)obj)->priv->data.code);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_is_parcel", "O", value) )
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
    if ( Py_TYPE(obj) == getProductDataType() ) {

        do_cpy(self->priv->code, ((ProductData*)obj)->priv->data.code);

    }

    else 
    if ( Py_TYPE(obj) == getProductDataKey0Type() ) {

        do_cpy(self->priv->code, ((ProductDataKey0*)obj)->priv->code);

    }

    else 
    if ( Py_TYPE(obj) == getProductViewType() ) {

        do_cpy(self->priv->code, ((ProductView*)obj)->priv->data.code);

    }

    else 
    if ( Py_TYPE(obj) == getProductViewKey0Type() ) {

        do_cpy(self->priv->code, ((ProductViewKey0*)obj)->priv->code);

    }

    else 
    if ( Py_TYPE(obj) == getProductViewKey1Type() ) {

        do_cpy(self->priv->code, ((ProductViewKey1*)obj)->priv->code);

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
            if ( !strcmp("is_parcel", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_is_parcel", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_code", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"ProductKey0\"does not contain \"%s\"", field_name);
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

static PyObject *ProductKey0_fields(ProductKey0* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class ProductKey0");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  ProductKey0_is_parcel(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("is_parcel", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  ProductKey0_code(self,NULL);
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

static PyGetSetDef ProductKey0_getset[] = {

    {"is_parcel",(getter)ProductKey0_is_parcel},

    {"code",(getter)ProductKey0_code},

    {NULL}
};

static PyMethodDef ProductKey0_methods[] = {
    {"set", (PyCFunction)ProductKey0_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)ProductKey0_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)ProductKey0_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)ProductKey0_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_is_parcel", (PyCFunction)ProductKey0_set_is_parcel, METH_VARARGS|METH_KEYWORDS, "ProductKey0_set_is_parcel"},

    {"set_code", (PyCFunction)ProductKey0_set_code, METH_VARARGS|METH_KEYWORDS, "ProductKey0_set_code"},

    {"get_prev", (PyCFunction)ProductKey0_prev, METH_VARARGS|METH_KEYWORDS, "ProductKey0_prev"},

    {"get_gt", (PyCFunction)ProductKey0_gt, METH_VARARGS|METH_KEYWORDS, "ProductKey0_gt"},

    {"get_next", (PyCFunction)ProductKey0_next, METH_VARARGS|METH_KEYWORDS, "ProductKey0_next"},

    {"get_ge", (PyCFunction)ProductKey0_ge, METH_VARARGS|METH_KEYWORDS, "ProductKey0_ge"},

    {"get_equal", (PyCFunction)ProductKey0_equal, METH_VARARGS|METH_KEYWORDS, "ProductKey0_equal"},

    {"get_last", (PyCFunction)ProductKey0_last, METH_VARARGS|METH_KEYWORDS, "ProductKey0_last"},

    {"get_lt", (PyCFunction)ProductKey0_lt, METH_VARARGS|METH_KEYWORDS, "ProductKey0_lt"},

    {"get_le", (PyCFunction)ProductKey0_le, METH_VARARGS|METH_KEYWORDS, "ProductKey0_le"},

    {"get_first", (PyCFunction)ProductKey0_first, METH_VARARGS|METH_KEYWORDS, "ProductKey0_first"},

    {"gets_gt", (PyCFunction)ProductKey0_iter_gt, METH_VARARGS|METH_KEYWORDS, "ProductKey0_iter_gt"},

    {"gets_ge", (PyCFunction)ProductKey0_iter_ge, METH_VARARGS|METH_KEYWORDS, "ProductKey0_iter_ge"},

    {"gets_equal", (PyCFunction)ProductKey0_iter_equal, METH_VARARGS|METH_KEYWORDS, "ProductKey0_iter_equal"},

    {"gets_last", (PyCFunction)ProductKey0_iter_last, METH_VARARGS|METH_KEYWORDS, "ProductKey0_iter_last"},

    {"gets_lt", (PyCFunction)ProductKey0_iter_lt, METH_VARARGS|METH_KEYWORDS, "ProductKey0_iter_lt"},

    {"gets_le", (PyCFunction)ProductKey0_iter_le, METH_VARARGS|METH_KEYWORDS, "ProductKey0_iter_le"},

    {"gets_first", (PyCFunction)ProductKey0_iter_first, METH_VARARGS|METH_KEYWORDS, "ProductKey0_iter_first"},

    {NULL}
};

static PyTypeObject ProductKey0Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.ProductKey0",             /*tp_name*/
    sizeof(ProductKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ProductKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)ProductKey0_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ProductKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ProductKey0_methods,             /* tp_methods */
    0,
    ProductKey0_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ProductKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    ProductKey0_new,                 /* tp_new */
};
PyTypeObject *getProductKey0Type()
{
    return &ProductKey0Type_;
}

static void ProductKey1_dealloc(PyObject* self)
{
    do_free(((ProductKey1*)self)->priv);
    Py_DECREF(((ProductKey1*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("ProductKey1 free\n");
}
static PyObject *ProductKey1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("ProductKey1 new\n");
    ProductKey1 *self;
    self = PyObject_NEW(ProductKey1, type);
    //self = (ProductKey1 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(product_key1_t));
    return (PyObject *)self;
}
static int ProductKey1_init(ProductKey1 *self, PyObject *args, PyObject *kwds)
{
    //LOG("ProductKey1 init\n");
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

    do_text_set_empty(self->priv->name);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *ProductKey1_clone(ProductKey1 *self)
{
    ProductKey1 *result;
    result = (ProductKey1*)PyObject_CallFunction((PyObject*)getProductKey1Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *ProductKey1_is_parcel(ProductKey1* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->base_parcel);

    return result;
}

static PyObject *ProductKey1_set_is_parcel(ProductKey1* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set base_parcel");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->base_parcel = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->base_parcel = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set base_parcel");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->base_parcel);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *ProductKey1_class_id(ProductKey1* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->class_id);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ProductKey1_set_class_id(ProductKey1* self, PyObject *args, PyObject *kwds)
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

static PyObject *ProductKey1_group_id(ProductKey1* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->group_id);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ProductKey1_set_group_id(ProductKey1* self, PyObject *args, PyObject *kwds)
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

static PyObject *ProductKey1_subgroup_id(ProductKey1* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->subgroup_id);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ProductKey1_set_subgroup_id(ProductKey1* self, PyObject *args, PyObject *kwds)
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

static PyObject *ProductKey1_name(ProductKey1* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->name);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ProductKey1_set_name(ProductKey1* self, PyObject *args, PyObject *kwds)
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

static PyObject *ProductKey1_prev(ProductKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey1_gt(ProductKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key1(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey1_next(ProductKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key1(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey1_ge(ProductKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key1(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey1_equal(ProductKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key1(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey1_last(ProductKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key1(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey1_lt(ProductKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key1(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey1_le(ProductKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key1(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey1_first(ProductKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key1(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey1_iter_gt(ProductKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_key1(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.base_parcel, 
                 self->priv->base_parcel))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.class_id, 
                 self->priv->class_id))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.group_id, 
                 self->priv->group_id))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.subgroup_id, 
                 self->priv->subgroup_id))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = ProductKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductKey1_iter_ge(ProductKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_key1(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.base_parcel, 
                 self->priv->base_parcel))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.class_id, 
                 self->priv->class_id))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.group_id, 
                 self->priv->group_id))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.subgroup_id, 
                 self->priv->subgroup_id))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = ProductKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductKey1_iter_equal(ProductKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_key1(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.base_parcel, 
                 self->priv->base_parcel))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.class_id, 
                 self->priv->class_id))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.group_id, 
                 self->priv->group_id))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.subgroup_id, 
                 self->priv->subgroup_id))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = ProductKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductKey1_iter_last(ProductKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_key1(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.base_parcel, 
                 self->priv->base_parcel))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.class_id, 
                 self->priv->class_id))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.group_id, 
                 self->priv->group_id))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.subgroup_id, 
                 self->priv->subgroup_id))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = ProductKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductKey1_iter_lt(ProductKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_key1(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.base_parcel, 
                 self->priv->base_parcel))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.class_id, 
                 self->priv->class_id))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.group_id, 
                 self->priv->group_id))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.subgroup_id, 
                 self->priv->subgroup_id))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = ProductKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductKey1_iter_le(ProductKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_key1(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.base_parcel, 
                 self->priv->base_parcel))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.class_id, 
                 self->priv->class_id))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.group_id, 
                 self->priv->group_id))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.subgroup_id, 
                 self->priv->subgroup_id))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = ProductKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductKey1_iter_first(ProductKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_key1(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.base_parcel, 
                 self->priv->base_parcel))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.class_id, 
                 self->priv->class_id))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.group_id, 
                 self->priv->group_id))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.subgroup_id, 
                 self->priv->subgroup_id))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = ProductKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductKey1_cmp(ProductKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getProductKey1Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = self->priv->base_parcel - ((ProductKey1*)obj)->priv->base_parcel;
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->class_id, ((ProductKey1*)obj)->priv->class_id);
       }

       if ( !res && depth >= 3 ) {
           res = do_cmp(self->priv->group_id, ((ProductKey1*)obj)->priv->group_id);
       }

       if ( !res && depth >= 4 ) {
           res = do_cmp(self->priv->subgroup_id, ((ProductKey1*)obj)->priv->subgroup_id);
       }

       if ( !res && depth >= 5 ) {
           res = do_cmp(self->priv->name, ((ProductKey1*)obj)->priv->name);
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *ProductKey1_set_alias(ProductKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((ProductKey1*)self)->alias);
    Py_INCREF(obj);
    ((ProductKey1*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *ProductKey1_set(ProductKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getProductKey1Type() ) {
        memcpy(self->priv, ((ProductKey1*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getProductType() ) {

        do_cpy(self->priv->base_parcel, ((Product*)obj)->priv->data.base_parcel);

        do_cpy(self->priv->class_id, ((Product*)obj)->priv->data.class_id);

        do_cpy(self->priv->group_id, ((Product*)obj)->priv->data.group_id);

        do_cpy(self->priv->subgroup_id, ((Product*)obj)->priv->data.subgroup_id);

        do_cpy(self->priv->name, ((Product*)obj)->priv->data.name);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_is_parcel", "O", value) )
                        return NULL;
                    break;

                case 1:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_class_id", "O", value) )
                        return NULL;
                    break;

                case 2:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_group_id", "O", value) )
                        return NULL;
                    break;

                case 3:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_subgroup_id", "O", value) )
                        return NULL;
                    break;

                case 4:
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
    if ( Py_TYPE(obj) == getProductDataType() ) {

    }

    else 
    if ( Py_TYPE(obj) == getProductDataKey0Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getProductViewType() ) {

    }

    else 
    if ( Py_TYPE(obj) == getProductViewKey0Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getProductViewKey1Type() ) {

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
            if ( !strcmp("is_parcel", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_is_parcel", "O", value) )
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

            else
            if ( !strcmp("name", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_name", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"ProductKey1\"does not contain \"%s\"", field_name);
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

static PyObject *ProductKey1_fields(ProductKey1* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class ProductKey1");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  ProductKey1_is_parcel(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("is_parcel", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  ProductKey1_class_id(self,NULL);
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

    value =  ProductKey1_group_id(self,NULL);
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

    value =  ProductKey1_subgroup_id(self,NULL);
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

    value =  ProductKey1_name(self,NULL);
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

static PyGetSetDef ProductKey1_getset[] = {

    {"is_parcel",(getter)ProductKey1_is_parcel},

    {"class_id",(getter)ProductKey1_class_id},

    {"group_id",(getter)ProductKey1_group_id},

    {"subgroup_id",(getter)ProductKey1_subgroup_id},

    {"name",(getter)ProductKey1_name},

    {NULL}
};

static PyMethodDef ProductKey1_methods[] = {
    {"set", (PyCFunction)ProductKey1_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)ProductKey1_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)ProductKey1_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)ProductKey1_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_is_parcel", (PyCFunction)ProductKey1_set_is_parcel, METH_VARARGS|METH_KEYWORDS, "ProductKey1_set_is_parcel"},

    {"set_class_id", (PyCFunction)ProductKey1_set_class_id, METH_VARARGS|METH_KEYWORDS, "ProductKey1_set_class_id"},

    {"set_group_id", (PyCFunction)ProductKey1_set_group_id, METH_VARARGS|METH_KEYWORDS, "ProductKey1_set_group_id"},

    {"set_subgroup_id", (PyCFunction)ProductKey1_set_subgroup_id, METH_VARARGS|METH_KEYWORDS, "ProductKey1_set_subgroup_id"},

    {"set_name", (PyCFunction)ProductKey1_set_name, METH_VARARGS|METH_KEYWORDS, "ProductKey1_set_name"},

    {"get_prev", (PyCFunction)ProductKey1_prev, METH_VARARGS|METH_KEYWORDS, "ProductKey1_prev"},

    {"get_gt", (PyCFunction)ProductKey1_gt, METH_VARARGS|METH_KEYWORDS, "ProductKey1_gt"},

    {"get_next", (PyCFunction)ProductKey1_next, METH_VARARGS|METH_KEYWORDS, "ProductKey1_next"},

    {"get_ge", (PyCFunction)ProductKey1_ge, METH_VARARGS|METH_KEYWORDS, "ProductKey1_ge"},

    {"get_equal", (PyCFunction)ProductKey1_equal, METH_VARARGS|METH_KEYWORDS, "ProductKey1_equal"},

    {"get_last", (PyCFunction)ProductKey1_last, METH_VARARGS|METH_KEYWORDS, "ProductKey1_last"},

    {"get_lt", (PyCFunction)ProductKey1_lt, METH_VARARGS|METH_KEYWORDS, "ProductKey1_lt"},

    {"get_le", (PyCFunction)ProductKey1_le, METH_VARARGS|METH_KEYWORDS, "ProductKey1_le"},

    {"get_first", (PyCFunction)ProductKey1_first, METH_VARARGS|METH_KEYWORDS, "ProductKey1_first"},

    {"gets_gt", (PyCFunction)ProductKey1_iter_gt, METH_VARARGS|METH_KEYWORDS, "ProductKey1_iter_gt"},

    {"gets_ge", (PyCFunction)ProductKey1_iter_ge, METH_VARARGS|METH_KEYWORDS, "ProductKey1_iter_ge"},

    {"gets_equal", (PyCFunction)ProductKey1_iter_equal, METH_VARARGS|METH_KEYWORDS, "ProductKey1_iter_equal"},

    {"gets_last", (PyCFunction)ProductKey1_iter_last, METH_VARARGS|METH_KEYWORDS, "ProductKey1_iter_last"},

    {"gets_lt", (PyCFunction)ProductKey1_iter_lt, METH_VARARGS|METH_KEYWORDS, "ProductKey1_iter_lt"},

    {"gets_le", (PyCFunction)ProductKey1_iter_le, METH_VARARGS|METH_KEYWORDS, "ProductKey1_iter_le"},

    {"gets_first", (PyCFunction)ProductKey1_iter_first, METH_VARARGS|METH_KEYWORDS, "ProductKey1_iter_first"},

    {NULL}
};

static PyTypeObject ProductKey1Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.ProductKey1",             /*tp_name*/
    sizeof(ProductKey1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ProductKey1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)ProductKey1_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ProductKey1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ProductKey1_methods,             /* tp_methods */
    0,
    ProductKey1_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ProductKey1_init,      /* tp_init */
    0,                         /* tp_alloc */
    ProductKey1_new,                 /* tp_new */
};
PyTypeObject *getProductKey1Type()
{
    return &ProductKey1Type_;
}

static void ProductKey2_dealloc(PyObject* self)
{
    do_free(((ProductKey2*)self)->priv);
    Py_DECREF(((ProductKey2*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("ProductKey2 free\n");
}
static PyObject *ProductKey2_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("ProductKey2 new\n");
    ProductKey2 *self;
    self = PyObject_NEW(ProductKey2, type);
    //self = (ProductKey2 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(product_key2_t));
    return (PyObject *)self;
}
static int ProductKey2_init(ProductKey2 *self, PyObject *args, PyObject *kwds)
{
    //LOG("ProductKey2 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->article);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *ProductKey2_clone(ProductKey2 *self)
{
    ProductKey2 *result;
    result = (ProductKey2*)PyObject_CallFunction((PyObject*)getProductKey2Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *ProductKey2_is_parcel(ProductKey2* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->base_parcel);

    return result;
}

static PyObject *ProductKey2_set_is_parcel(ProductKey2* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set base_parcel");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->base_parcel = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->base_parcel = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set base_parcel");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->base_parcel);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *ProductKey2_article(ProductKey2* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->article);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ProductKey2_set_article(ProductKey2* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set article");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->article, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->article);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *ProductKey2_prev(ProductKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey2_gt(ProductKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key2(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey2_next(ProductKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key2(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey2_ge(ProductKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key2(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey2_equal(ProductKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key2(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey2_last(ProductKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key2(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey2_lt(ProductKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key2(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey2_le(ProductKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key2(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey2_first(ProductKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key2(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey2_iter_gt(ProductKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_key2(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.base_parcel, 
                 self->priv->base_parcel))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.article, 
                 self->priv->article))
               break;
       }

 
        item = ProductKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductKey2_iter_ge(ProductKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_key2(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.base_parcel, 
                 self->priv->base_parcel))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.article, 
                 self->priv->article))
               break;
       }

 
        item = ProductKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductKey2_iter_equal(ProductKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_key2(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.base_parcel, 
                 self->priv->base_parcel))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.article, 
                 self->priv->article))
               break;
       }

 
        item = ProductKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductKey2_iter_last(ProductKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_key2(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.base_parcel, 
                 self->priv->base_parcel))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.article, 
                 self->priv->article))
               break;
       }

 
        item = ProductKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductKey2_iter_lt(ProductKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_key2(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.base_parcel, 
                 self->priv->base_parcel))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.article, 
                 self->priv->article))
               break;
       }

 
        item = ProductKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductKey2_iter_le(ProductKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_key2(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.base_parcel, 
                 self->priv->base_parcel))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.article, 
                 self->priv->article))
               break;
       }

 
        item = ProductKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductKey2_iter_first(ProductKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_key2(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.base_parcel, 
                 self->priv->base_parcel))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.article, 
                 self->priv->article))
               break;
       }

 
        item = ProductKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductKey2_cmp(ProductKey2* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getProductKey2Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = self->priv->base_parcel - ((ProductKey2*)obj)->priv->base_parcel;
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->article, ((ProductKey2*)obj)->priv->article);
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *ProductKey2_set_alias(ProductKey2* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((ProductKey2*)self)->alias);
    Py_INCREF(obj);
    ((ProductKey2*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *ProductKey2_set(ProductKey2* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getProductKey2Type() ) {
        memcpy(self->priv, ((ProductKey2*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getProductType() ) {

        do_cpy(self->priv->base_parcel, ((Product*)obj)->priv->data.base_parcel);

        do_cpy(self->priv->article, ((Product*)obj)->priv->data.article);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_is_parcel", "O", value) )
                        return NULL;
                    break;

                case 1:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_article", "O", value) )
                        return NULL;
                    break;

                default:
                    do_log(LOG_ERR, "%s size larger than the number of fields", PyList_Check(obj) ? "List":"Tuple");
                    return NULL;
            }
        }
    }

    else 
    if ( Py_TYPE(obj) == getProductDataType() ) {

    }

    else 
    if ( Py_TYPE(obj) == getProductDataKey0Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getProductViewType() ) {

    }

    else 
    if ( Py_TYPE(obj) == getProductViewKey0Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getProductViewKey1Type() ) {

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
            if ( !strcmp("is_parcel", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_is_parcel", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("article", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_article", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"ProductKey2\"does not contain \"%s\"", field_name);
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

static PyObject *ProductKey2_fields(ProductKey2* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class ProductKey2");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  ProductKey2_is_parcel(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("is_parcel", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  ProductKey2_article(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("article", value);
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

static PyGetSetDef ProductKey2_getset[] = {

    {"is_parcel",(getter)ProductKey2_is_parcel},

    {"article",(getter)ProductKey2_article},

    {NULL}
};

static PyMethodDef ProductKey2_methods[] = {
    {"set", (PyCFunction)ProductKey2_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)ProductKey2_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)ProductKey2_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)ProductKey2_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_is_parcel", (PyCFunction)ProductKey2_set_is_parcel, METH_VARARGS|METH_KEYWORDS, "ProductKey2_set_is_parcel"},

    {"set_article", (PyCFunction)ProductKey2_set_article, METH_VARARGS|METH_KEYWORDS, "ProductKey2_set_article"},

    {"get_prev", (PyCFunction)ProductKey2_prev, METH_VARARGS|METH_KEYWORDS, "ProductKey2_prev"},

    {"get_gt", (PyCFunction)ProductKey2_gt, METH_VARARGS|METH_KEYWORDS, "ProductKey2_gt"},

    {"get_next", (PyCFunction)ProductKey2_next, METH_VARARGS|METH_KEYWORDS, "ProductKey2_next"},

    {"get_ge", (PyCFunction)ProductKey2_ge, METH_VARARGS|METH_KEYWORDS, "ProductKey2_ge"},

    {"get_equal", (PyCFunction)ProductKey2_equal, METH_VARARGS|METH_KEYWORDS, "ProductKey2_equal"},

    {"get_last", (PyCFunction)ProductKey2_last, METH_VARARGS|METH_KEYWORDS, "ProductKey2_last"},

    {"get_lt", (PyCFunction)ProductKey2_lt, METH_VARARGS|METH_KEYWORDS, "ProductKey2_lt"},

    {"get_le", (PyCFunction)ProductKey2_le, METH_VARARGS|METH_KEYWORDS, "ProductKey2_le"},

    {"get_first", (PyCFunction)ProductKey2_first, METH_VARARGS|METH_KEYWORDS, "ProductKey2_first"},

    {"gets_gt", (PyCFunction)ProductKey2_iter_gt, METH_VARARGS|METH_KEYWORDS, "ProductKey2_iter_gt"},

    {"gets_ge", (PyCFunction)ProductKey2_iter_ge, METH_VARARGS|METH_KEYWORDS, "ProductKey2_iter_ge"},

    {"gets_equal", (PyCFunction)ProductKey2_iter_equal, METH_VARARGS|METH_KEYWORDS, "ProductKey2_iter_equal"},

    {"gets_last", (PyCFunction)ProductKey2_iter_last, METH_VARARGS|METH_KEYWORDS, "ProductKey2_iter_last"},

    {"gets_lt", (PyCFunction)ProductKey2_iter_lt, METH_VARARGS|METH_KEYWORDS, "ProductKey2_iter_lt"},

    {"gets_le", (PyCFunction)ProductKey2_iter_le, METH_VARARGS|METH_KEYWORDS, "ProductKey2_iter_le"},

    {"gets_first", (PyCFunction)ProductKey2_iter_first, METH_VARARGS|METH_KEYWORDS, "ProductKey2_iter_first"},

    {NULL}
};

static PyTypeObject ProductKey2Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.ProductKey2",             /*tp_name*/
    sizeof(ProductKey2),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ProductKey2_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)ProductKey2_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ProductKey2 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ProductKey2_methods,             /* tp_methods */
    0,
    ProductKey2_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ProductKey2_init,      /* tp_init */
    0,                         /* tp_alloc */
    ProductKey2_new,                 /* tp_new */
};
PyTypeObject *getProductKey2Type()
{
    return &ProductKey2Type_;
}

static void ProductKey3_dealloc(PyObject* self)
{
    do_free(((ProductKey3*)self)->priv);
    Py_DECREF(((ProductKey3*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("ProductKey3 free\n");
}
static PyObject *ProductKey3_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("ProductKey3 new\n");
    ProductKey3 *self;
    self = PyObject_NEW(ProductKey3, type);
    //self = (ProductKey3 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(product_key3_t));
    return (PyObject *)self;
}
static int ProductKey3_init(ProductKey3 *self, PyObject *args, PyObject *kwds)
{
    //LOG("ProductKey3 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->supplier_region);

    do_text_set_empty(self->priv->party);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *ProductKey3_clone(ProductKey3 *self)
{
    ProductKey3 *result;
    result = (ProductKey3*)PyObject_CallFunction((PyObject*)getProductKey3Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *ProductKey3_is_parcel(ProductKey3* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->base_parcel);

    return result;
}

static PyObject *ProductKey3_set_is_parcel(ProductKey3* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set base_parcel");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->base_parcel = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->base_parcel = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set base_parcel");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->base_parcel);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *ProductKey3_supplier_region_code(ProductKey3* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->supplier_region);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ProductKey3_set_supplier_region_code(ProductKey3* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set supplier_region");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->supplier_region, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->supplier_region);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *ProductKey3_supplier_code(ProductKey3* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->supplier_code);

    return result;
}

static PyObject *ProductKey3_set_supplier_code(ProductKey3* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set supplier_code");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->supplier_code = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->supplier_code = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set supplier_code");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->supplier_code);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *ProductKey3_party(ProductKey3* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->party);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ProductKey3_set_party(ProductKey3* self, PyObject *args, PyObject *kwds)
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

static PyObject *ProductKey3_prev(ProductKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key3(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey3_gt(ProductKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key3(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey3_next(ProductKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key3(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey3_ge(ProductKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key3(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey3_equal(ProductKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key3(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey3_last(ProductKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key3(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey3_lt(ProductKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key3(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey3_le(ProductKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key3(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey3_first(ProductKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key3(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey3_iter_gt(ProductKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_key3(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.base_parcel, 
                 self->priv->base_parcel))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.supplier_region, 
                 self->priv->supplier_region))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.supplier_code, 
                 self->priv->supplier_code))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.party, 
                 self->priv->party))
               break;
       }

 
        item = ProductKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_key3(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductKey3_iter_ge(ProductKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_key3(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.base_parcel, 
                 self->priv->base_parcel))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.supplier_region, 
                 self->priv->supplier_region))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.supplier_code, 
                 self->priv->supplier_code))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.party, 
                 self->priv->party))
               break;
       }

 
        item = ProductKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_key3(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductKey3_iter_equal(ProductKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_key3(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.base_parcel, 
                 self->priv->base_parcel))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.supplier_region, 
                 self->priv->supplier_region))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.supplier_code, 
                 self->priv->supplier_code))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.party, 
                 self->priv->party))
               break;
       }

 
        item = ProductKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_key3(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductKey3_iter_last(ProductKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_key3(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.base_parcel, 
                 self->priv->base_parcel))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.supplier_region, 
                 self->priv->supplier_region))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.supplier_code, 
                 self->priv->supplier_code))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.party, 
                 self->priv->party))
               break;
       }

 
        item = ProductKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_key3(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductKey3_iter_lt(ProductKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_key3(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.base_parcel, 
                 self->priv->base_parcel))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.supplier_region, 
                 self->priv->supplier_region))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.supplier_code, 
                 self->priv->supplier_code))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.party, 
                 self->priv->party))
               break;
       }

 
        item = ProductKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_key3(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductKey3_iter_le(ProductKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_key3(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.base_parcel, 
                 self->priv->base_parcel))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.supplier_region, 
                 self->priv->supplier_region))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.supplier_code, 
                 self->priv->supplier_code))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.party, 
                 self->priv->party))
               break;
       }

 
        item = ProductKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_key3(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductKey3_iter_first(ProductKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_key3(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.base_parcel, 
                 self->priv->base_parcel))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.supplier_region, 
                 self->priv->supplier_region))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.supplier_code, 
                 self->priv->supplier_code))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.party, 
                 self->priv->party))
               break;
       }

 
        item = ProductKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_key3(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductKey3_cmp(ProductKey3* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getProductKey3Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = self->priv->base_parcel - ((ProductKey3*)obj)->priv->base_parcel;
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->supplier_region, ((ProductKey3*)obj)->priv->supplier_region);
       }

       if ( !res && depth >= 3 ) {
           res = self->priv->supplier_code - ((ProductKey3*)obj)->priv->supplier_code;
       }

       if ( !res && depth >= 4 ) {
           res = do_cmp(self->priv->party, ((ProductKey3*)obj)->priv->party);
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *ProductKey3_set_alias(ProductKey3* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((ProductKey3*)self)->alias);
    Py_INCREF(obj);
    ((ProductKey3*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *ProductKey3_set(ProductKey3* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getProductKey3Type() ) {
        memcpy(self->priv, ((ProductKey3*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getProductType() ) {

        do_cpy(self->priv->base_parcel, ((Product*)obj)->priv->data.base_parcel);

        do_cpy(self->priv->supplier_region, ((Product*)obj)->priv->data.supplier_region);

        do_cpy(self->priv->supplier_code, ((Product*)obj)->priv->data.supplier_code);

        do_cpy(self->priv->party, ((Product*)obj)->priv->data.party);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_is_parcel", "O", value) )
                        return NULL;
                    break;

                case 1:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_supplier_region_code", "O", value) )
                        return NULL;
                    break;

                case 2:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_supplier_code", "O", value) )
                        return NULL;
                    break;

                case 3:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_party", "O", value) )
                        return NULL;
                    break;

                default:
                    do_log(LOG_ERR, "%s size larger than the number of fields", PyList_Check(obj) ? "List":"Tuple");
                    return NULL;
            }
        }
    }

    else 
    if ( Py_TYPE(obj) == getProductDataType() ) {

    }

    else 
    if ( Py_TYPE(obj) == getProductDataKey0Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getProductViewType() ) {

    }

    else 
    if ( Py_TYPE(obj) == getProductViewKey0Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getProductViewKey1Type() ) {

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
            if ( !strcmp("is_parcel", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_is_parcel", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("supplier_region_code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_supplier_region_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("supplier_code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_supplier_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("party", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_party", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"ProductKey3\"does not contain \"%s\"", field_name);
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

static PyObject *ProductKey3_fields(ProductKey3* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class ProductKey3");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  ProductKey3_is_parcel(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("is_parcel", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  ProductKey3_supplier_region_code(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("supplier_region_code", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  ProductKey3_supplier_code(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("supplier_code", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  ProductKey3_party(self,NULL);
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
    
    buf = do_strdup_printf("%s\n",str);
    retval = MyString_FromString(buf);
    do_free(str);
    do_free(buf);
    return retval;
}

static PyGetSetDef ProductKey3_getset[] = {

    {"is_parcel",(getter)ProductKey3_is_parcel},

    {"supplier_region_code",(getter)ProductKey3_supplier_region_code},

    {"supplier_code",(getter)ProductKey3_supplier_code},

    {"party",(getter)ProductKey3_party},

    {NULL}
};

static PyMethodDef ProductKey3_methods[] = {
    {"set", (PyCFunction)ProductKey3_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)ProductKey3_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)ProductKey3_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)ProductKey3_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_is_parcel", (PyCFunction)ProductKey3_set_is_parcel, METH_VARARGS|METH_KEYWORDS, "ProductKey3_set_is_parcel"},

    {"set_supplier_region_code", (PyCFunction)ProductKey3_set_supplier_region_code, METH_VARARGS|METH_KEYWORDS, "ProductKey3_set_supplier_region_code"},

    {"set_supplier_code", (PyCFunction)ProductKey3_set_supplier_code, METH_VARARGS|METH_KEYWORDS, "ProductKey3_set_supplier_code"},

    {"set_party", (PyCFunction)ProductKey3_set_party, METH_VARARGS|METH_KEYWORDS, "ProductKey3_set_party"},

    {"get_prev", (PyCFunction)ProductKey3_prev, METH_VARARGS|METH_KEYWORDS, "ProductKey3_prev"},

    {"get_gt", (PyCFunction)ProductKey3_gt, METH_VARARGS|METH_KEYWORDS, "ProductKey3_gt"},

    {"get_next", (PyCFunction)ProductKey3_next, METH_VARARGS|METH_KEYWORDS, "ProductKey3_next"},

    {"get_ge", (PyCFunction)ProductKey3_ge, METH_VARARGS|METH_KEYWORDS, "ProductKey3_ge"},

    {"get_equal", (PyCFunction)ProductKey3_equal, METH_VARARGS|METH_KEYWORDS, "ProductKey3_equal"},

    {"get_last", (PyCFunction)ProductKey3_last, METH_VARARGS|METH_KEYWORDS, "ProductKey3_last"},

    {"get_lt", (PyCFunction)ProductKey3_lt, METH_VARARGS|METH_KEYWORDS, "ProductKey3_lt"},

    {"get_le", (PyCFunction)ProductKey3_le, METH_VARARGS|METH_KEYWORDS, "ProductKey3_le"},

    {"get_first", (PyCFunction)ProductKey3_first, METH_VARARGS|METH_KEYWORDS, "ProductKey3_first"},

    {"gets_gt", (PyCFunction)ProductKey3_iter_gt, METH_VARARGS|METH_KEYWORDS, "ProductKey3_iter_gt"},

    {"gets_ge", (PyCFunction)ProductKey3_iter_ge, METH_VARARGS|METH_KEYWORDS, "ProductKey3_iter_ge"},

    {"gets_equal", (PyCFunction)ProductKey3_iter_equal, METH_VARARGS|METH_KEYWORDS, "ProductKey3_iter_equal"},

    {"gets_last", (PyCFunction)ProductKey3_iter_last, METH_VARARGS|METH_KEYWORDS, "ProductKey3_iter_last"},

    {"gets_lt", (PyCFunction)ProductKey3_iter_lt, METH_VARARGS|METH_KEYWORDS, "ProductKey3_iter_lt"},

    {"gets_le", (PyCFunction)ProductKey3_iter_le, METH_VARARGS|METH_KEYWORDS, "ProductKey3_iter_le"},

    {"gets_first", (PyCFunction)ProductKey3_iter_first, METH_VARARGS|METH_KEYWORDS, "ProductKey3_iter_first"},

    {NULL}
};

static PyTypeObject ProductKey3Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.ProductKey3",             /*tp_name*/
    sizeof(ProductKey3),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ProductKey3_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)ProductKey3_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ProductKey3 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ProductKey3_methods,             /* tp_methods */
    0,
    ProductKey3_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ProductKey3_init,      /* tp_init */
    0,                         /* tp_alloc */
    ProductKey3_new,                 /* tp_new */
};
PyTypeObject *getProductKey3Type()
{
    return &ProductKey3Type_;
}

static void ProductKey4_dealloc(PyObject* self)
{
    do_free(((ProductKey4*)self)->priv);
    Py_DECREF(((ProductKey4*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("ProductKey4 free\n");
}
static PyObject *ProductKey4_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("ProductKey4 new\n");
    ProductKey4 *self;
    self = PyObject_NEW(ProductKey4, type);
    //self = (ProductKey4 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(product_key4_t));
    return (PyObject *)self;
}
static int ProductKey4_init(ProductKey4 *self, PyObject *args, PyObject *kwds)
{
    //LOG("ProductKey4 init\n");
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

static PyObject *ProductKey4_clone(ProductKey4 *self)
{
    ProductKey4 *result;
    result = (ProductKey4*)PyObject_CallFunction((PyObject*)getProductKey4Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *ProductKey4_code(ProductKey4* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ProductKey4_set_code(ProductKey4* self, PyObject *args, PyObject *kwds)
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

static PyObject *ProductKey4_prev(ProductKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key4(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey4_gt(ProductKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key4(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey4_next(ProductKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key4(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey4_ge(ProductKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key4(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey4_equal(ProductKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key4(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey4_last(ProductKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key4(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey4_lt(ProductKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key4(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey4_le(ProductKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key4(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey4_first(ProductKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_key4(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductKey4_iter_gt(ProductKey4* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_key4_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_key4(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = ProductKey4_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_key4(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductKey4_iter_ge(ProductKey4* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_key4_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_key4(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = ProductKey4_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_key4(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductKey4_iter_equal(ProductKey4* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_key4_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_key4(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = ProductKey4_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_key4(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductKey4_iter_last(ProductKey4* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_key4_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_key4(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = ProductKey4_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_key4(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductKey4_iter_lt(ProductKey4* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_key4_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_key4(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = ProductKey4_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_key4(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductKey4_iter_le(ProductKey4* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_key4_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_key4(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = ProductKey4_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_key4(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductKey4_iter_first(ProductKey4* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_key4_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_key4(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = ProductKey4_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_key4(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductKey4_cmp(ProductKey4* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getProductKey4Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->code, ((ProductKey4*)obj)->priv->code);
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *ProductKey4_set_alias(ProductKey4* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((ProductKey4*)self)->alias);
    Py_INCREF(obj);
    ((ProductKey4*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *ProductKey4_set(ProductKey4* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getProductKey4Type() ) {
        memcpy(self->priv, ((ProductKey4*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getProductType() ) {

        do_cpy(self->priv->code, ((Product*)obj)->priv->data.code);

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
    if ( Py_TYPE(obj) == getProductDataType() ) {

        do_cpy(self->priv->code, ((ProductData*)obj)->priv->data.code);

    }

    else 
    if ( Py_TYPE(obj) == getProductDataKey0Type() ) {

        do_cpy(self->priv->code, ((ProductDataKey0*)obj)->priv->code);

    }

    else 
    if ( Py_TYPE(obj) == getProductViewType() ) {

        do_cpy(self->priv->code, ((ProductView*)obj)->priv->data.code);

    }

    else 
    if ( Py_TYPE(obj) == getProductViewKey0Type() ) {

        do_cpy(self->priv->code, ((ProductViewKey0*)obj)->priv->code);

    }

    else 
    if ( Py_TYPE(obj) == getProductViewKey1Type() ) {

        do_cpy(self->priv->code, ((ProductViewKey1*)obj)->priv->code);

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
                do_log(LOG_ERR, "Class \"ProductKey4\"does not contain \"%s\"", field_name);
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

static PyObject *ProductKey4_fields(ProductKey4* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class ProductKey4");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  ProductKey4_code(self,NULL);
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

static PyGetSetDef ProductKey4_getset[] = {

    {"code",(getter)ProductKey4_code},

    {NULL}
};

static PyMethodDef ProductKey4_methods[] = {
    {"set", (PyCFunction)ProductKey4_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)ProductKey4_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)ProductKey4_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)ProductKey4_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_code", (PyCFunction)ProductKey4_set_code, METH_VARARGS|METH_KEYWORDS, "ProductKey4_set_code"},

    {"get_prev", (PyCFunction)ProductKey4_prev, METH_VARARGS|METH_KEYWORDS, "ProductKey4_prev"},

    {"get_gt", (PyCFunction)ProductKey4_gt, METH_VARARGS|METH_KEYWORDS, "ProductKey4_gt"},

    {"get_next", (PyCFunction)ProductKey4_next, METH_VARARGS|METH_KEYWORDS, "ProductKey4_next"},

    {"get_ge", (PyCFunction)ProductKey4_ge, METH_VARARGS|METH_KEYWORDS, "ProductKey4_ge"},

    {"get_equal", (PyCFunction)ProductKey4_equal, METH_VARARGS|METH_KEYWORDS, "ProductKey4_equal"},

    {"get_last", (PyCFunction)ProductKey4_last, METH_VARARGS|METH_KEYWORDS, "ProductKey4_last"},

    {"get_lt", (PyCFunction)ProductKey4_lt, METH_VARARGS|METH_KEYWORDS, "ProductKey4_lt"},

    {"get_le", (PyCFunction)ProductKey4_le, METH_VARARGS|METH_KEYWORDS, "ProductKey4_le"},

    {"get_first", (PyCFunction)ProductKey4_first, METH_VARARGS|METH_KEYWORDS, "ProductKey4_first"},

    {"gets_gt", (PyCFunction)ProductKey4_iter_gt, METH_VARARGS|METH_KEYWORDS, "ProductKey4_iter_gt"},

    {"gets_ge", (PyCFunction)ProductKey4_iter_ge, METH_VARARGS|METH_KEYWORDS, "ProductKey4_iter_ge"},

    {"gets_equal", (PyCFunction)ProductKey4_iter_equal, METH_VARARGS|METH_KEYWORDS, "ProductKey4_iter_equal"},

    {"gets_last", (PyCFunction)ProductKey4_iter_last, METH_VARARGS|METH_KEYWORDS, "ProductKey4_iter_last"},

    {"gets_lt", (PyCFunction)ProductKey4_iter_lt, METH_VARARGS|METH_KEYWORDS, "ProductKey4_iter_lt"},

    {"gets_le", (PyCFunction)ProductKey4_iter_le, METH_VARARGS|METH_KEYWORDS, "ProductKey4_iter_le"},

    {"gets_first", (PyCFunction)ProductKey4_iter_first, METH_VARARGS|METH_KEYWORDS, "ProductKey4_iter_first"},

    {NULL}
};

static PyTypeObject ProductKey4Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.ProductKey4",             /*tp_name*/
    sizeof(ProductKey4),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ProductKey4_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)ProductKey4_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ProductKey4 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ProductKey4_methods,             /* tp_methods */
    0,
    ProductKey4_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ProductKey4_init,      /* tp_init */
    0,                         /* tp_alloc */
    ProductKey4_new,                 /* tp_new */
};
PyTypeObject *getProductKey4Type()
{
    return &ProductKey4Type_;
}
