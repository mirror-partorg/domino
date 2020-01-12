
#include "markup.h"
#include <datetime.h>
#include "../var/db.h"
#include "../var/util.h"
#include "price_limit.h"
#include "markup_info.h"
#include "misc.h"

struct _MarkupPrivate
{
	Alias *obj_alias;
	do_alias_t *alias;
	//markup_info_t *info;
};

static void Markup_dealloc(PyObject* self)
{
    LOG("Markup free\n");
    if ( ((Markup*)self)->priv->obj_alias )
	    Py_DECREF(((Markup*)self)->priv->obj_alias);
	else
        if ( ((Markup*)self)->priv->alias )
            do_alias_free(((Markup*)self)->priv->alias);
    /*if ( ((Markup*)self)->priv->info )
        do_markup_free(((Markup*)self)->priv->info);*/
    do_free(((Markup*)self)->priv);

    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *Markup_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("Markup new\n");
    Markup *self;
    self = PyObject_NEW(Markup, type);
    //self = (Markup *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(MarkupPrivate));
    return (PyObject *)self;
}
static int Markup_init(Markup *self, PyObject *args, PyObject *kwds)
{
    //LOG("Markup init\n");
    static char *kwlist[] = {"alias", "alias_name", NULL};
    PyObject *obj_alias = NULL;
    char *alias_name = NULL;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|Os", kwlist, &obj_alias, &alias_name) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }
    if ( obj_alias ) {
        if ( Py_TYPE(obj_alias) != getAliasType() ) {
            do_log(LOG_ERR, "no param \"alias\"");
            return -1;
        }
        Py_INCREF(obj_alias);
        self->priv->obj_alias = (Alias*)obj_alias;
        self->priv->alias = ((Alias*)obj_alias)->alias;
    }
    else {
        self->priv->alias = domino_alias_new(alias_name);
        if ( !self->priv->alias )
            return -1;
        do_alias_open(self->priv->alias, TRUE);
    }
    PyDateTime_IMPORT;
    return 0;
}
static PyObject *Markup_max_price_grls(Markup *self, PyObject *args, PyObject *kwds);
static PyObject *Markup_max_pct_grls(Markup *self, PyObject *args, PyObject *kwds);

static PyObject *Markup_get(Markup *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"product", "store", "supplier",  NULL};
    PyObject *product = NULL;
    PyObject *store = NULL;
    PyObject *supplier = NULL;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|OO", kwlist, &product, &store, &supplier) ) {
        do_log(LOG_ERR, "no param \"product\"");
        return NULL;
    }
    char *code;
    code = get_product_code(product);
    if ( !code ) {
        do_log(LOG_ERR, "Invalid argument \"product\": wrong type");
        return NULL;
    }
    char *unit;
    if ( store ) {
        unit = get_store_code(store);
        if ( !unit ) {
            do_log(LOG_ERR, "Invalid argument \"store\": wrong type");
            return NULL;
        }
    }
    else {
        unit = do_strdup(domino_unit());
    }

    char *supplier_region = NULL;
    int supplier_code;
    if ( supplier ) {
        supplier_region = get_partner_code(supplier, &supplier_code);
        if ( !supplier ) {
            do_log(LOG_ERR, "Invalid argument \"supplier\": wrong type");
            return NULL;
        }
    }
    markup_info_t *info;
    info = do_markup_get(self->priv->alias, unit, code, supplier_region, supplier_code);
    do_free(code);
    do_free(unit);
    if ( supplier_region) do_free(supplier_region);
    if ( !info ) {
        Py_INCREF(Py_None);
        return Py_None;
    }
    PyObject *item;
    item = PyObject_CallFunction((PyObject*)getMarkupInfoType(),"");
    ((MarkupInfo*)item)->info = info;
    return item;
}
static PyObject *Markup_get_grls(Markup *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"store", NULL};
    PyObject *store = NULL;
    do_alias_t *alias = self->priv->alias;
    
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &store) ) {
        do_log(LOG_ERR, "no param \"store\"");
        return NULL;
    }
    char *unit;
    if ( store ) {
        unit = get_store_code(store);
        if ( !unit ) {
            do_log(LOG_ERR, "Invalid argument \"store\": wrong type");
            return NULL;
        }
    }
    else {
        unit = do_strdup(domino_unit());
    }
    
    sklad_key0_t sklad_key0;
    sklad_rec_t sklad;
    partner_key0_t partner_key0;
    partner_rec_t partner;
    do_string_list_t *l;
    int i;

    do_text_set(alias, sklad_key0.code, unit);
    if ( do_sklad_get0(alias, &sklad, &sklad_key0, DO_GET_EQUAL) != DO_OK ) {
        do_log(LOG_ERR,"Store \"%s\" not found", unit);
        return NULL;
    }
    do_cpy(partner_key0.g_code, sklad.data.p_g_code);
    partner_key0.code = sklad.data.p_code;

    if ( do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL) != DO_OK ) {
        do_log(LOG_ERR,"Partner \"%s.%d\" not found", do_text(alias, partner_key0.g_code), partner_key0.code);
        return NULL;
    }
    char *value, *ch;
    value = do_partner_param(alias, &partner, "CITY");
    ch = strchr(value, '.');
    if ( !ch ) {
        do_free(value);
        do_log(LOG_ERR, "not specified a city in partner  \"%s.%d\"", do_text(alias, partner_key0.g_code), partner_key0.code);
        return NULL;
    }
    do_text_set(alias, partner_key0.g_code, value);
    ch++;
    partner_key0.code = atoi(ch);
    do_free(value);
    if ( do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL) != DO_OK ) {
        do_log(LOG_ERR,"Partner \"%s.%d\" not found", do_text(alias, partner_key0.g_code), partner_key0.code);
        return NULL;
    }

//OptPrice = Округлить (m2*(1+markup/100.)*(1+СтавкаНДС/100.),10);
//MaxPrice = Округлить (OptPrice + m2*(markupretail/100.)*(1+СтавкаНДС/100.),10);

    value = do_partner_param(alias, &partner, "SCALE");
    l = do_string_list_new_from_csv(value);
    do_free(value);
    PyObject *scale_list;
    scale_list = PyList_New(0);
    for ( i = 0; i < l->count; i+=2 ) {
        double top;
        double rate;
        top = atof(l->list[i]);
        rate = atof(l->list[i+1]);
        PyObject *item;
        item = PyTuple_New(2);
        PyTuple_SET_ITEM(item, 0, PyFloat_FromDouble(top));
        PyTuple_SET_ITEM(item, 1, PyFloat_FromDouble(rate));
        PyList_Append(scale_list, (PyObject*)item);
        Py_DECREF(item);
    }

    do_string_list_free(l);
    value = do_partner_param(alias, &partner, "SCALERETAIL");
    l = do_string_list_new_from_csv(value);
    do_free(value);
    PyObject *scale_retail_list;
    scale_retail_list = PyList_New(0);
    for ( i = 0; i < l->count; i+=2 ) {
        double top;
        double rate;
        top = atof(l->list[i]);
        rate = atof(l->list[i+1]);
        PyObject *item;
        item = PyTuple_New(2);
        PyTuple_SET_ITEM(item, 0, PyFloat_FromDouble(top));
        PyTuple_SET_ITEM(item, 1, PyFloat_FromDouble(rate));
        PyList_Append(scale_retail_list, (PyObject*)item);
        Py_DECREF(item);
    }
    do_string_list_free(l);
    
    PyObject *result;
    result = PyTuple_New(2);
    PyTuple_SET_ITEM(result, 0, scale_list);
    PyTuple_SET_ITEM(result, 1, scale_retail_list);
    //Py_DECREF(scale_list);
    //Py_DECREF(max_list);
    return result;
}


static PyMethodDef Markup_methods[] = {
    {"get", (PyCFunction)Markup_get, METH_VARARGS|METH_KEYWORDS, "get"},
    {"get_grls", (PyCFunction)Markup_get_grls, METH_VARARGS|METH_KEYWORDS, "get_grls"},
    {"grls_get_maxprice", (PyCFunction)Markup_max_price_grls, METH_VARARGS|METH_KEYWORDS, "max_price_grls"},
    {"grls_get_maxpct", (PyCFunction)Markup_max_pct_grls, METH_VARARGS|METH_KEYWORDS, "max_pct_grls"},
    {NULL}
};

static PyTypeObject MarkupType_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.Markup",             /*tp_name*/
    sizeof(Markup),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Markup_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Markup objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Markup_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Markup_init,      /* tp_init */
    0,                         /* tp_alloc */
    Markup_new,                 /* tp_new */
};
PyTypeObject *getMarkupType()
{
    return &MarkupType_;
}
static PyObject *Markup_max_price_grls(Markup *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"price", "store", "vat_rate", "product", "barcode",  NULL};
    PyObject *product = NULL;
    PyObject *store = NULL;
    PyObject *barcode = NULL;
    PyObject *price = NULL;
    int vat_rate = -1;
    char *product_code = NULL;
    char *unit_code = NULL;
    char *barcode_code = NULL;
    int price_len = 0;
    double *prices = NULL;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|OOiOO", kwlist, &price, &store, &vat_rate, &product, &barcode) ) {
        do_log(LOG_ERR, "no correct param ");
        return NULL;
    }
    if  ( product ) {
        product_code = get_product_code(product);
        if ( !product_code ) {
            do_log(LOG_ERR, "Invalid argument \"product\": wrong type");
            return NULL;
        }
        if ( vat_rate == -1 && Py_TYPE(product) == getProductType() ) {
            Product *p = (Product *)product;
            vat_rate = do_product_param_int(p->alias->alias, p->priv, do_param(DO_PARAM_PRODUCT_PARAMETR_NDS));
        }
        if ( strlen(product_code) > do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH_DEPRECATE) &&
             product_code[do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH_DEPRECATE)] == '.' )
            product_code[do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH_DEPRECATE)] = '\0';
    }
    if ( vat_rate == -1 && product_code ) {
        product_rec_t product;
        product_key0_t product_key0;
        do_text_set(self->priv->alias, product_key0.code, product_code);
        product_key0.base_parcel = 0;
        if ( do_product_get0(self->priv->alias, &product, &product_key0, DO_GET_EQUAL) != DO_OK ) {
            do_log(LOG_WARNING, "Product with code \"%s\" not found", product_code);
            return PyFloat_FromDouble(0);
        }
        vat_rate = do_product_param_int(self->priv->alias, &product, do_param(DO_PARAM_PRODUCT_PARAMETR_NDS));
    }
    if ( store ) {
        unit_code = get_store_code(store);
        if ( !unit_code ) {
            do_log(LOG_ERR, "Invalid argument \"store\": wrong type");
            return NULL;
        }
    }
    else {
        unit_code = do_strdup(domino_unit());
    }
    if ( barcode ) {
        barcode_code = get_barcode_code(barcode);
        if ( !barcode_code ) {
            do_log(LOG_ERR, "Invalid argument \"barcode\": wrong type");
            return NULL;
        }
    }
    if ( price ) {
        if ( PyFloat_Check(price) ) {
            prices = do_malloc0(sizeof(double));
            prices[0] = PyFloat_AsDouble(price);
            price_len = 1;
        }
        else
        if ( MyLong_Check(price) ) {
            prices = do_malloc0(sizeof(double));
            prices[0] = MyLong_AsLong(price);
            price_len = 1;
        }
        else
        if ( PyList_Check(price) || PyTuple_Check(price) ) {
            int i;
            price_len = PyList_Check(price) ? PyList_Size(price) : PyTuple_Size(price);
            prices = do_malloc0(sizeof(double)*price_len);
            for ( i = 0; i < price_len; i++ ) {
                PyObject *value;
                value =  PyList_Check(price) ? PyList_GetItem(price, i) : PyTuple_GetItem(price, i);
                if ( PyFloat_Check(value) )
                    prices[i] = PyFloat_AsDouble(value);
                else
                if ( MyLong_Check(value) )
                    prices[i] = MyLong_AsLong(value);
                else {
                    do_free(prices);
                    do_log(LOG_ERR, "Invalid argument \"price\": wrong type");
                    return NULL;
                }
            }
        }
        else {
            do_log(LOG_ERR, "Invalid argument \"price\": wrong type");
            return NULL;
        }
    }

    if ( vat_rate == -1 )
        vat_rate = 10;

    if ( !price_len && !product_code && !barcode_code ) {
        do_log(LOG_ERR, "no param \"price\" or \"product\" or \"barcode\"");
        return NULL;
    }
    if ( !price_len ) {
        double price_reg_min = 0, price_reg_max = 0;
        do_list_t *l = NULL;
        int i;
        rz_info_t *item;

        if ( barcode_code )
            l = do_rz_info_list_from_barcode(self->priv->alias, barcode_code);
        else
        if (product_code )
            l = do_rz_info_list_from_code(self->priv->alias, product_code);
        if ( l ) {
            for ( i = 0; i < l->count; i++ ) {
                item = l->list[i];
                if ( !price_reg_min || price_reg_min > item->price )
                    price_reg_min = item->price;
                if ( !price_reg_max || price_reg_max < item->price )
                    price_reg_max = item->price;
            }
            do_rz_info_list_free(l);
            if ( price_reg_min && price_reg_max ) {
                price_len=2;
                prices = do_malloc0(sizeof(double)*price_len);
                prices[0] = price_reg_min;
                prices[1] = price_reg_max;
            }
        }
    }
    if ( !price_len ) {
        do_log(LOG_WARNING, "Can't determined register price");
        return PyFloat_FromDouble(0);
    }
    double retval;
    retval = do_markup_get_max_grls_price_(self->priv->alias, unit_code, vat_rate, price_len, prices);
    do_free(prices);

    return PyFloat_FromDouble(retval);
}
static PyObject *Markup_max_pct_grls(Markup *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"price", "store", "product", "barcode",  NULL};
    PyObject *product = NULL;
    PyObject *store = NULL;
    PyObject *barcode = NULL;
    PyObject *price = NULL;
    char *product_code = NULL;
    char *unit_code = NULL;
    char *barcode_code = NULL;
    int price_len = 0;
    double *prices = NULL;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|OOOO", kwlist, &price, &store, &product, &barcode) ) {
        do_log(LOG_ERR, "no correct param ");
        return NULL;
    }
    if  ( product ) {
        product_code = get_product_code(product);
        if ( !product_code ) {
            do_log(LOG_ERR, "Invalid argument \"product\": wrong type");
            return NULL;
        }
        if ( strlen(product_code) > do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH_DEPRECATE) &&
             product_code[do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH_DEPRECATE)] == '.' )
            product_code[do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH_DEPRECATE)] = '\0';
    }
    if ( store ) {
        unit_code = get_store_code(store);
        if ( !unit_code ) {
            do_log(LOG_ERR, "Invalid argument \"store\": wrong type");
            return NULL;
        }
    }
    else {
        unit_code = do_strdup(domino_unit());
    }
    if ( barcode ) {
        barcode_code = get_barcode_code(barcode);
        if ( !barcode_code ) {
            do_log(LOG_ERR, "Invalid argument \"barcode\": wrong type");
            return NULL;
        }
    }
    if ( price ) {
        if ( PyFloat_Check(price) ) {
            prices = do_malloc0(sizeof(double));
            prices[0] = PyFloat_AsDouble(price);
            price_len = 1;
        }
        else
        if ( MyLong_Check(price) ) {
            prices = do_malloc0(sizeof(double));
            prices[0] = MyLong_AsLong(price);
            price_len = 1;
        }
        else
        if ( PyList_Check(price) || PyTuple_Check(price) ) {
            int i;
            price_len = PyList_Check(price) ? PyList_Size(price) : PyTuple_Size(price);
            prices = do_malloc0(sizeof(double)*price_len);
            for ( i = 0; i < price_len; i++ ) {
                PyObject *value;
                value =  PyList_Check(price) ? PyList_GetItem(price, i) : PyTuple_GetItem(price, i);
                if ( PyFloat_Check(value) )
                    prices[i] = PyFloat_AsDouble(value);
                else
                if ( MyLong_Check(value) )
                    prices[i] = MyLong_AsLong(value);
                else {
                    do_free(prices);
                    do_log(LOG_ERR, "Invalid argument \"price\": wrong type");
                    return NULL;
                }
            }
        }
        else {
            do_log(LOG_ERR, "Invalid argument \"price\": wrong type");
            return NULL;
        }
    }

    if ( !price_len && !product_code && !barcode_code ) {
        do_log(LOG_ERR, "no param \"price\" or \"product\" or \"barcode\"");
        return NULL;
    }
    if ( !price_len ) {
        double price_reg_min = 0, price_reg_max = 0;
        do_list_t *l = NULL;
        int i;
        rz_info_t *item;

        if ( barcode_code )
            l = do_rz_info_list_from_barcode(self->priv->alias, barcode_code);
        else
        if ( product_code )
            l = do_rz_info_list_from_code(self->priv->alias, product_code);
        if ( l ) {
            for ( i = 0; i < l->count; i++ ) {
                item = l->list[i];
                if ( !price_reg_min || price_reg_min > item->price )
                    price_reg_min = item->price;
                if ( !price_reg_max || price_reg_max < item->price )
                    price_reg_max = item->price;
            }
            do_rz_info_list_free(l);
            if ( price_reg_min && price_reg_max ) {
                price_len=2;
                prices = do_malloc0(sizeof(double)*price_len);
                prices[0] = price_reg_min;
                prices[1] = price_reg_max;
            }
        }
    }
    if ( !price_len || !price ) {
        do_log(LOG_WARNING, "Can't determined register price");
        return PyFloat_FromDouble(0);
    }
    double pct1,pct2;
    do_markup_get_max_grls_pct_(self->priv->alias, unit_code, &pct1, &pct2, price_len, prices);
    do_free(prices);
    PyObject *ret, *obj;
    ret = PyList_New(0);
    obj = PyFloat_FromDouble(pct1);
    PyList_Append(ret, obj);
    Py_DECREF(obj);
    obj = PyFloat_FromDouble(pct2);
    PyList_Append(ret, obj);
    Py_DECREF(obj);
    return ret;
}
