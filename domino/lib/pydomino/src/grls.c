#ifndef DOMINO78

#include "grls.h"
#include <datetime.h>
#include "../var/db.h"
#include "../var/util.h"
#include "price_limit.h"
#include "misc.h"

struct _GrlsPrivate
{
	Alias *obj_alias;
	do_alias_t *alias;
};

static void Grls_dealloc(PyObject* self)
{
    LOG("Grls free\n");
    if ( ((Grls*)self)->priv->obj_alias )
	    Py_DECREF(((Grls*)self)->priv->obj_alias);
	else
        if ( ((Grls*)self)->priv->alias )
            do_alias_free(((Grls*)self)->priv->alias);
    do_free(((Grls*)self)->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *Grls_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("Grls new\n");
    Grls *self;
    self = PyObject_NEW(Grls, type);
    //self = (Grls *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(GrlsPrivate));
    return (PyObject *)self;
}
static int Grls_init(Grls *self, PyObject *args, PyObject *kwds)
{
    //LOG("Grls init\n");
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

static PyObject *get_price_limits(do_list_t *list);
static PyObject *Grls_price_barcode(Grls *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"barcode", NULL};
    PyObject *barcode = NULL;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O", kwlist, &barcode) ) {
        do_log(LOG_ERR, "no param \"barcode\"");
        return NULL;
    }
    char *bar;
    bar = get_barcode_code(barcode);
    if ( !bar ) {
        do_log(LOG_ERR, "Invalid argument \"barcode\": wrong type");
        return NULL;
    }

    PyObject *res;
    res = get_price_limits(do_rz_info_list_from_barcode(self->priv->alias, bar));
    do_free(bar);
    return res;
}
static PyObject *Grls_price_code(Grls *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"product", NULL};
    PyObject *product = NULL;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O", kwlist, &product) ) {
        do_log(LOG_ERR, "no param \"product\"");
        return NULL;
    }
    char *code;
    code = get_product_code(product);
    if ( !code ) {
        do_log(LOG_ERR, "Invalid argument \"product\": wrong type");
        return NULL;
    }

    PyObject *res;
    res = get_price_limits(do_rz_info_list_from_code(self->priv->alias, code));
    do_free(code);
    return res;
}


static PyMethodDef Grls_methods[] = {
    {"get_price_limits_by_barcode", (PyCFunction)Grls_price_barcode, METH_VARARGS|METH_KEYWORDS, "get_price_limits_by_barcode"},
    {"get_price_limits_by_code", (PyCFunction)Grls_price_code, METH_VARARGS|METH_KEYWORDS, "get_price_limits_by_code"},

    {NULL}
};

static PyTypeObject GrlsType_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.Grls",             /*tp_name*/
    sizeof(Grls),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Grls_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0/*(reprfunc)Grls_fields*/,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Grls objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Grls_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Grls_init,      /* tp_init */
    0,                         /* tp_alloc */
    Grls_new,                 /* tp_new */
};
PyTypeObject *getGrlsType()
{
    return &GrlsType_;
}

static PyObject *get_price_limits(do_list_t *list)
{
    if ( !list )
        return NULL;

    PyObject *res;
    int i;
    res = PyList_New(0);
    for ( i = 0; i < list->count; i++ ) {
        PyObject *item;
        item = PyObject_CallFunction((PyObject*)getPriceLimitType(),"");
        ((PriceLimit*)item)->info = list->list[i];
        //Py_INCREF(item);
        PyList_Append(res, item);
    }
    do_list_free(list);
    //Py_INCREF(res);
    return res;
}
#endif