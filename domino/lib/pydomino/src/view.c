
#include "view.h"
#include <datetime.h>
#include "../var/db.h"
#include "../var/util.h"
#include "price_limit.h"
#include "misc.h"

struct _ViewPrivate
{
	Alias *obj_alias;
	do_alias_t *alias;
	do_ret_list_t *list_product_view;
};

typedef struct PACK_STRUCT {
    BTI_CHAR code[DOMINO_FIELD_CODE_SIZE];
    BTI_CHAR sort[40];
} rec_product_view_t;

static void View_dealloc(PyObject* self)
{
    LOG("View free\n");
    if ( ((View*)self)->priv->obj_alias )
        Py_DECREF(((View*)self)->priv->obj_alias);
    else
        if ( ((View*)self)->priv->alias )
            do_alias_free(((View*)self)->priv->alias);
    if ( ((View*)self)->priv->list_product_view )
        do_ret_list_free(((View*)self)->priv->list_product_view);
    do_free(((View*)self)->priv);

    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *View_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("View new\n");
    View *self;
    self = PyObject_NEW(View, type);
    //self = (View *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(ViewPrivate));
    return (PyObject *)self;
}
static int View_init(View *self, PyObject *args, PyObject *kwds)
{
    //LOG("View init\n");
    static char *kwlist[] = {"alias", "alias_name", NULL};
    PyObject *obj_alias = NULL;
    char *alias_name = NULL;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|Os", kwlist, &obj_alias, &alias_name) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        LOG("View init fail\n");
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
static do_ret_list_t *View_get_product_view(do_alias_t *alias)
{
    do_ret_list_t       *retval;
    product_view_key0_t  product_view_key0;
    do_extended_t       *extended;

    do_text_set_empty(product_view_key0.code);
    do_text_set_empty(product_view_key0.sort);
    do_text_set_empty(product_view_key0.sklad);
    product_view_key0.type = 1;

    extended = do_extended_new(alias);
    do_extended_set_file_db(extended, DO_DB_PRODUCT_VIEW);
    do_extended_set_key(extended, 0, &product_view_key0);
    do_extended_set_filter(extended, FALSE, 1, sizeof(product_view_key0.type));
    do_extended_set_filter_term(extended, 0,
                                DO_EXTENDED_TYPE_INTEGER,
                                offsetof(product_view_struct_t, type),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_NO,
                                &product_view_key0.type);
    do_extended_set_fields(extended, 2,
                           do_size_and_offset(product_view_struct_t, code),
                           do_size_and_offset(product_view_struct_t, sort)
                           );
    retval = do_extended_get(extended, -1);
    do_extended_free(extended);
    return retval;
}
static PyObject *View_get_product_count(View *self) 
{

    do_alias_t *alias;
    alias = self->priv->alias;
    if ( !self->priv->list_product_view )
        self->priv->list_product_view = View_get_product_view(alias);
    if ( self->priv->list_product_view )
        return PyLong_FromLong(do_ret_list_count(self->priv->list_product_view));
    else {
        Py_INCREF(Py_None);
        return Py_None;
    }
}
static PyObject *View_get_product_list(View *self) 
{

    do_alias_t *alias;
    
    alias = self->priv->alias;
    if ( !self->priv->list_product_view )
        self->priv->list_product_view = View_get_product_view(alias);
    
    if ( !self->priv->list_product_view ) {
        Py_INCREF(Py_None);
        return Py_None;
    }
    PyObject *retval;
    PyObject *item;
    int i;
    char *value;
    rec_product_view_t *rec;
    retval = PyList_New(0);
    for ( i = 0; i < do_ret_list_count(self->priv->list_product_view); i++ ) {
         rec = do_ret_list_item_data(self->priv->list_product_view, i);
         item = PyTuple_New(2);
         value = do_text(alias, rec->code);
         PyTuple_SET_ITEM(item, 0, MyString_FromString(value));
         do_free(value);
         value = do_text(alias, rec->sort);
         PyTuple_SET_ITEM(item, 1, MyString_FromString(value));
         do_free(value);
         PyList_Append(retval, (PyObject*)item);
         Py_DECREF(item);        
    }
    return retval;
}
static PyObject *View_update_product(View *self) 
{

    do_alias_t *alias;
    alias = self->priv->alias;
    if ( self->priv->list_product_view )
        do_ret_list_free(self->priv->list_product_view);
    self->priv->list_product_view = View_get_product_view(alias);
    return PyBool_FromLong(self->priv->list_product_view != NULL);
}

static PyObject *View_get_product_code(View *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"index", NULL};
    int index = 0;
    do_alias_t *alias;
    alias = self->priv->alias;
    rec_product_view_t *rec;
    char *value;
    PyObject *result;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i", kwlist, &index) ) {
        do_log(LOG_ERR, "no param \"index\"");
        return NULL;
    }
    if ( !self->priv->list_product_view )
        self->priv->list_product_view = View_get_product_view(alias);
    
    if ( !self->priv->list_product_view || index < 0 || index >= do_ret_list_count(self->priv->list_product_view) ) {
        Py_INCREF(Py_None);
        return Py_None;
    }
    rec = do_ret_list_item_data(self->priv->list_product_view, index);
    value = do_text(alias, rec->code);
    result = MyString_FromString(value);
    do_free(value);
    return result;
}
static PyObject *View_get_product_sort(View *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"index", NULL};
    int index = 0;
    do_alias_t *alias;
    alias = self->priv->alias;
    rec_product_view_t *rec;
    char *value;
    PyObject *result;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i", kwlist, &index) ) {
        do_log(LOG_ERR, "no param \"index\"");
        return NULL;
    }
    if ( !self->priv->list_product_view )
        self->priv->list_product_view = View_get_product_view(alias);
    if ( !self->priv->list_product_view || index < 0 || index >= do_ret_list_count(self->priv->list_product_view) ) {
        Py_INCREF(Py_None);
        return Py_None;
    }
    rec = do_ret_list_item_data(self->priv->list_product_view, index);
    value = do_text(alias, rec->sort);
    result = MyString_FromString(value);
    do_free(value);
    return result;
}



static PyMethodDef View_methods[] = {
    {"get_product_count", (PyCFunction)View_get_product_count, METH_NOARGS, "get-product-count"},
    {"update_product", (PyCFunction)View_update_product, METH_NOARGS, "update-product"},
    {"get_product_list", (PyCFunction)View_get_product_list, METH_NOARGS, "get-product-list"},
    {"get_product_code", (PyCFunction)View_get_product_code, METH_VARARGS|METH_KEYWORDS, "get-product-code"},
    {"get_product_sort", (PyCFunction)View_get_product_sort, METH_VARARGS|METH_KEYWORDS, "get-product-sort"},
    {NULL}
};

static PyTypeObject ViewType_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.View",             /*tp_name*/
    sizeof(View),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)View_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "View objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    View_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)View_init,      /* tp_init */
    0,                         /* tp_alloc */
    View_new,                 /* tp_new */
};
PyTypeObject *getViewType()
{
    return &ViewType_;
}
