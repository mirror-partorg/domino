#include <Python.h>
//#include <structmember.h>
#include <domino.h>
#include <locale.h>
#include "pydomino.h"

static PyObject *DominoError;

#ifdef _DEBUG
#define LOG(...) fprintf (stderr, __VA_ARGS__)
#else
#define LOG(...)
#endif // _DEBUG

static void Alias_dealloc(Alias* self)
{
    LOG("Alias free\n");
    do_alias_free(self->alias);
    self->ob_type->tp_free((PyObject*)self);
}

static PyObject *Alias_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Alias *self;

    self = (Alias *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->alias = NULL;
    LOG("Alias new\n");
    return (PyObject *)self;
}

static int Alias_init(Alias *self, PyObject *args, PyObject *kwds)
{
    LOG("Alias init\n");

    static char *kwlist[] = {"name", NULL};
    char *name = NULL;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist,
                                      &name) )
        return -1;
    self->alias = domino_alias_new(name);
    return 0;
}

static PyObject *Alias_connect(Alias *self)
{
    LOG("Alias connect\n");

    if ( !do_alias_open(self->alias, TRUE) )
        return NULL;

    Py_INCREF(Py_None);
    return Py_None;
}


/*static PyMemberDef Alias_members[] = {
    {NULL}  // Sentinel
};*/

static PyObject *Alias_get_name(Alias* self)
{
    PyObject *result;

    result = PyString_FromString(do_alias_get_name(self->alias));

    return result;
}
static PyObject *Alias_get_host(Alias* self)
{
    PyObject *result;

    result = PyString_FromString(do_alias_get_host(self->alias));

    return result;
}
static PyObject *Alias_utf8(Alias* self)
{
    PyObject  *result;

    result = Py_BuildValue("i", do_alias_utf8(self->alias));

    return result;
}
static PyObject *Alias_connected(Alias* self)
{
    PyObject  *result;

    result = Py_BuildValue("i", do_alias_conected(self->alias));

    return result;
}

static PyMethodDef Alias_methods[] = {
    {"get_name", (PyCFunction)Alias_get_name, METH_NOARGS,
     "do_alias_get_name"
    },
    {"get_host", (PyCFunction)Alias_get_host, METH_NOARGS,
     "do_alias_get_host"
    },
    {"utf8", (PyCFunction)Alias_utf8, METH_NOARGS,
     "do_alias_utf8"
    },
    {"connect", (PyCFunction)Alias_connect, METH_NOARGS,// METH_VARARGS|METH_KEYWORDS,
     "do_alias_open"
    },
    {"connected", (PyCFunction)Alias_connected, METH_NOARGS,// METH_VARARGS|METH_KEYWORDS,
     "do_alias_connected"
    },
    {NULL}  /* Sentinel */
};

static PyTypeObject AliasType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.Alias",             /*tp_name*/
    sizeof(Alias),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Alias_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Alias objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    Alias_methods,             /* tp_methods */
    0,//Alias_members,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)Alias_init,      /* tp_init */
    0,                         /* tp_alloc */
    Alias_new,                 /* tp_new */
};

static PyMethodDef module_methods[] = {
    {NULL}  /* Sentinel */
};

#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif
static int log_func(const char *mgs, int level, void *user_data)
{
    LOG("!!!!!!log error %s\n", mgs);
    if ( level == LOG_ERR || level == LOG_CRIT ) {
        PyErr_SetString(DominoError, mgs);
        return FALSE;
    }
    fprintf(stdout, "%s\n", mgs);
    return TRUE;
}
static void DocumentKey0_dealloc(DocumentKey0* self)
{
    LOG("DocumentKey0 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *DocumentKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    DocumentKey0 *self;
    self = (DocumentKey0 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("DocumentKey0 new\n");
    return (PyObject *)self;
}
static int DocumentKey0_init(DocumentKey0 *self, PyObject *args, PyObject *kwds)
{
    LOG("DocumentKey0 init\n");
    self->priv = do_malloc0(sizeof(document_key0_t));
    return 0;
}

static void DocumentKey1_dealloc(DocumentKey1* self)
{
    LOG("DocumentKey1 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *DocumentKey1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    DocumentKey1 *self;
    self = (DocumentKey1 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("DocumentKey1 new\n");
    return (PyObject *)self;
}
static int DocumentKey1_init(DocumentKey1 *self, PyObject *args, PyObject *kwds)
{
    LOG("DocumentKey1 init\n");
    self->priv = do_malloc0(sizeof(document_key1_t));
    return 0;
}

static void DocumentKey2_dealloc(DocumentKey2* self)
{
    LOG("DocumentKey2 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *DocumentKey2_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    DocumentKey2 *self;
    self = (DocumentKey2 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("DocumentKey2 new\n");
    return (PyObject *)self;
}
static int DocumentKey2_init(DocumentKey2 *self, PyObject *args, PyObject *kwds)
{
    LOG("DocumentKey2 init\n");
    self->priv = do_malloc0(sizeof(document_key2_t));
    return 0;
}

static void DocumentKey3_dealloc(DocumentKey3* self)
{
    LOG("DocumentKey3 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *DocumentKey3_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    DocumentKey3 *self;
    self = (DocumentKey3 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("DocumentKey3 new\n");
    return (PyObject *)self;
}
static int DocumentKey3_init(DocumentKey3 *self, PyObject *args, PyObject *kwds)
{
    LOG("DocumentKey3 init\n");
    self->priv = do_malloc0(sizeof(document_key3_t));
    return 0;
}

static void Document_dealloc(Document* self)
{
    LOG("Document free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *Document_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Document *self;
    self = (Document *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("Document new\n");
    return (PyObject *)self;
}
static int Document_init(Document *self, PyObject *args, PyObject *kwds)
{
    LOG("Document init\n");
    self->priv = do_malloc0(sizeof(document_rec_t));
    return 0;
}

static void DocumentOrderKey0_dealloc(DocumentOrderKey0* self)
{
    LOG("DocumentOrderKey0 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *DocumentOrderKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    DocumentOrderKey0 *self;
    self = (DocumentOrderKey0 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("DocumentOrderKey0 new\n");
    return (PyObject *)self;
}
static int DocumentOrderKey0_init(DocumentOrderKey0 *self, PyObject *args, PyObject *kwds)
{
    LOG("DocumentOrderKey0 init\n");
    self->priv = do_malloc0(sizeof(document_order_key0_t));
    return 0;
}

static void DocumentOrderKey1_dealloc(DocumentOrderKey1* self)
{
    LOG("DocumentOrderKey1 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *DocumentOrderKey1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    DocumentOrderKey1 *self;
    self = (DocumentOrderKey1 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("DocumentOrderKey1 new\n");
    return (PyObject *)self;
}
static int DocumentOrderKey1_init(DocumentOrderKey1 *self, PyObject *args, PyObject *kwds)
{
    LOG("DocumentOrderKey1 init\n");
    self->priv = do_malloc0(sizeof(document_order_key1_t));
    return 0;
}

static void DocumentOrderKey2_dealloc(DocumentOrderKey2* self)
{
    LOG("DocumentOrderKey2 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *DocumentOrderKey2_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    DocumentOrderKey2 *self;
    self = (DocumentOrderKey2 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("DocumentOrderKey2 new\n");
    return (PyObject *)self;
}
static int DocumentOrderKey2_init(DocumentOrderKey2 *self, PyObject *args, PyObject *kwds)
{
    LOG("DocumentOrderKey2 init\n");
    self->priv = do_malloc0(sizeof(document_order_key2_t));
    return 0;
}

static void DocumentOrderKey3_dealloc(DocumentOrderKey3* self)
{
    LOG("DocumentOrderKey3 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *DocumentOrderKey3_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    DocumentOrderKey3 *self;
    self = (DocumentOrderKey3 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("DocumentOrderKey3 new\n");
    return (PyObject *)self;
}
static int DocumentOrderKey3_init(DocumentOrderKey3 *self, PyObject *args, PyObject *kwds)
{
    LOG("DocumentOrderKey3 init\n");
    self->priv = do_malloc0(sizeof(document_order_key3_t));
    return 0;
}

static void DocumentOrderKey4_dealloc(DocumentOrderKey4* self)
{
    LOG("DocumentOrderKey4 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *DocumentOrderKey4_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    DocumentOrderKey4 *self;
    self = (DocumentOrderKey4 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("DocumentOrderKey4 new\n");
    return (PyObject *)self;
}
static int DocumentOrderKey4_init(DocumentOrderKey4 *self, PyObject *args, PyObject *kwds)
{
    LOG("DocumentOrderKey4 init\n");
    self->priv = do_malloc0(sizeof(document_order_key4_t));
    return 0;
}

static void DocumentOrder_dealloc(DocumentOrder* self)
{
    LOG("DocumentOrder free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *DocumentOrder_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    DocumentOrder *self;
    self = (DocumentOrder *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("DocumentOrder new\n");
    return (PyObject *)self;
}
static int DocumentOrder_init(DocumentOrder *self, PyObject *args, PyObject *kwds)
{
    LOG("DocumentOrder init\n");
    self->priv = do_malloc0(sizeof(document_order_rec_t));
    return 0;
}

static void DocumentLinkKey0_dealloc(DocumentLinkKey0* self)
{
    LOG("DocumentLinkKey0 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *DocumentLinkKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    DocumentLinkKey0 *self;
    self = (DocumentLinkKey0 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("DocumentLinkKey0 new\n");
    return (PyObject *)self;
}
static int DocumentLinkKey0_init(DocumentLinkKey0 *self, PyObject *args, PyObject *kwds)
{
    LOG("DocumentLinkKey0 init\n");
    self->priv = do_malloc0(sizeof(document_link_key0_t));
    return 0;
}

static void DocumentLinkKey1_dealloc(DocumentLinkKey1* self)
{
    LOG("DocumentLinkKey1 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *DocumentLinkKey1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    DocumentLinkKey1 *self;
    self = (DocumentLinkKey1 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("DocumentLinkKey1 new\n");
    return (PyObject *)self;
}
static int DocumentLinkKey1_init(DocumentLinkKey1 *self, PyObject *args, PyObject *kwds)
{
    LOG("DocumentLinkKey1 init\n");
    self->priv = do_malloc0(sizeof(document_link_key1_t));
    return 0;
}

static void DocumentLink_dealloc(DocumentLink* self)
{
    LOG("DocumentLink free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *DocumentLink_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    DocumentLink *self;
    self = (DocumentLink *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("DocumentLink new\n");
    return (PyObject *)self;
}
static int DocumentLink_init(DocumentLink *self, PyObject *args, PyObject *kwds)
{
    LOG("DocumentLink init\n");
    self->priv = do_malloc0(sizeof(document_link_rec_t));
    return 0;
}

static void EnumKey0_dealloc(EnumKey0* self)
{
    LOG("EnumKey0 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *EnumKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    EnumKey0 *self;
    self = (EnumKey0 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("EnumKey0 new\n");
    return (PyObject *)self;
}
static int EnumKey0_init(EnumKey0 *self, PyObject *args, PyObject *kwds)
{
    LOG("EnumKey0 init\n");
    self->priv = do_malloc0(sizeof(enum_key0_t));
    return 0;
}

static void Enum_dealloc(Enum* self)
{
    LOG("Enum free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *Enum_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Enum *self;
    self = (Enum *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("Enum new\n");
    return (PyObject *)self;
}
static int Enum_init(Enum *self, PyObject *args, PyObject *kwds)
{
    LOG("Enum init\n");
    self->priv = do_malloc0(sizeof(enum_rec_t));
    return 0;
}

static void DocumentViewKey0_dealloc(DocumentViewKey0* self)
{
    LOG("DocumentViewKey0 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *DocumentViewKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    DocumentViewKey0 *self;
    self = (DocumentViewKey0 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("DocumentViewKey0 new\n");
    return (PyObject *)self;
}
static int DocumentViewKey0_init(DocumentViewKey0 *self, PyObject *args, PyObject *kwds)
{
    LOG("DocumentViewKey0 init\n");
    self->priv = do_malloc0(sizeof(document_view_key0_t));
    return 0;
}

static void DocumentViewKey1_dealloc(DocumentViewKey1* self)
{
    LOG("DocumentViewKey1 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *DocumentViewKey1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    DocumentViewKey1 *self;
    self = (DocumentViewKey1 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("DocumentViewKey1 new\n");
    return (PyObject *)self;
}
static int DocumentViewKey1_init(DocumentViewKey1 *self, PyObject *args, PyObject *kwds)
{
    LOG("DocumentViewKey1 init\n");
    self->priv = do_malloc0(sizeof(document_view_key1_t));
    return 0;
}

static void DocumentView_dealloc(DocumentView* self)
{
    LOG("DocumentView free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *DocumentView_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    DocumentView *self;
    self = (DocumentView *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("DocumentView new\n");
    return (PyObject *)self;
}
static int DocumentView_init(DocumentView *self, PyObject *args, PyObject *kwds)
{
    LOG("DocumentView init\n");
    self->priv = do_malloc0(sizeof(document_view_rec_t));
    return 0;
}

static void DocumentDataKey0_dealloc(DocumentDataKey0* self)
{
    LOG("DocumentDataKey0 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *DocumentDataKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    DocumentDataKey0 *self;
    self = (DocumentDataKey0 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("DocumentDataKey0 new\n");
    return (PyObject *)self;
}
static int DocumentDataKey0_init(DocumentDataKey0 *self, PyObject *args, PyObject *kwds)
{
    LOG("DocumentDataKey0 init\n");
    self->priv = do_malloc0(sizeof(document_data_key0_t));
    return 0;
}

static void DocumentData_dealloc(DocumentData* self)
{
    LOG("DocumentData free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *DocumentData_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    DocumentData *self;
    self = (DocumentData *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("DocumentData new\n");
    return (PyObject *)self;
}
static int DocumentData_init(DocumentData *self, PyObject *args, PyObject *kwds)
{
    LOG("DocumentData init\n");
    self->priv = do_malloc0(sizeof(document_data_rec_t));
    return 0;
}

static void Product_dealloc(Product* self)
{
    LOG("Product free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *Product_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Product *self;
    self = (Product *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("Product new\n");
    return (PyObject *)self;
}
static int Product_init(Product *self, PyObject *args, PyObject *kwds)
{
    LOG("Product init\n");
    self->priv = do_malloc0(sizeof(product_rec_t));
    return 0;
}

static void ProductLinkKey0_dealloc(ProductLinkKey0* self)
{
    LOG("ProductLinkKey0 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *ProductLinkKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    ProductLinkKey0 *self;
    self = (ProductLinkKey0 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("ProductLinkKey0 new\n");
    return (PyObject *)self;
}
static int ProductLinkKey0_init(ProductLinkKey0 *self, PyObject *args, PyObject *kwds)
{
    LOG("ProductLinkKey0 init\n");
    self->priv = do_malloc0(sizeof(product_link_key0_t));
    return 0;
}

static void ProductLinkKey1_dealloc(ProductLinkKey1* self)
{
    LOG("ProductLinkKey1 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *ProductLinkKey1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    ProductLinkKey1 *self;
    self = (ProductLinkKey1 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("ProductLinkKey1 new\n");
    return (PyObject *)self;
}
static int ProductLinkKey1_init(ProductLinkKey1 *self, PyObject *args, PyObject *kwds)
{
    LOG("ProductLinkKey1 init\n");
    self->priv = do_malloc0(sizeof(product_link_key1_t));
    return 0;
}

static void ProductLink_dealloc(ProductLink* self)
{
    LOG("ProductLink free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *ProductLink_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    ProductLink *self;
    self = (ProductLink *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("ProductLink new\n");
    return (PyObject *)self;
}
static int ProductLink_init(ProductLink *self, PyObject *args, PyObject *kwds)
{
    LOG("ProductLink init\n");
    self->priv = do_malloc0(sizeof(product_link_rec_t));
    return 0;
}

static void ProductViewKey0_dealloc(ProductViewKey0* self)
{
    LOG("ProductViewKey0 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *ProductViewKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    ProductViewKey0 *self;
    self = (ProductViewKey0 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("ProductViewKey0 new\n");
    return (PyObject *)self;
}
static int ProductViewKey0_init(ProductViewKey0 *self, PyObject *args, PyObject *kwds)
{
    LOG("ProductViewKey0 init\n");
    self->priv = do_malloc0(sizeof(product_view_key0_t));
    return 0;
}

static void ProductViewKey1_dealloc(ProductViewKey1* self)
{
    LOG("ProductViewKey1 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *ProductViewKey1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    ProductViewKey1 *self;
    self = (ProductViewKey1 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("ProductViewKey1 new\n");
    return (PyObject *)self;
}
static int ProductViewKey1_init(ProductViewKey1 *self, PyObject *args, PyObject *kwds)
{
    LOG("ProductViewKey1 init\n");
    self->priv = do_malloc0(sizeof(product_view_key1_t));
    return 0;
}

static void ProductView_dealloc(ProductView* self)
{
    LOG("ProductView free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *ProductView_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    ProductView *self;
    self = (ProductView *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("ProductView new\n");
    return (PyObject *)self;
}
static int ProductView_init(ProductView *self, PyObject *args, PyObject *kwds)
{
    LOG("ProductView init\n");
    self->priv = do_malloc0(sizeof(product_view_rec_t));
    return 0;
}

static void ProductDataKey0_dealloc(ProductDataKey0* self)
{
    LOG("ProductDataKey0 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *ProductDataKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    ProductDataKey0 *self;
    self = (ProductDataKey0 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("ProductDataKey0 new\n");
    return (PyObject *)self;
}
static int ProductDataKey0_init(ProductDataKey0 *self, PyObject *args, PyObject *kwds)
{
    LOG("ProductDataKey0 init\n");
    self->priv = do_malloc0(sizeof(product_data_key0_t));
    return 0;
}

static void ProductData_dealloc(ProductData* self)
{
    LOG("ProductData free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *ProductData_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    ProductData *self;
    self = (ProductData *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("ProductData new\n");
    return (PyObject *)self;
}
static int ProductData_init(ProductData *self, PyObject *args, PyObject *kwds)
{
    LOG("ProductData init\n");
    self->priv = do_malloc0(sizeof(product_data_rec_t));
    return 0;
}

static void BarcodeKey0_dealloc(BarcodeKey0* self)
{
    LOG("BarcodeKey0 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *BarcodeKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    BarcodeKey0 *self;
    self = (BarcodeKey0 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("BarcodeKey0 new\n");
    return (PyObject *)self;
}
static int BarcodeKey0_init(BarcodeKey0 *self, PyObject *args, PyObject *kwds)
{
    LOG("BarcodeKey0 init\n");
    self->priv = do_malloc0(sizeof(barcode_key0_t));
    return 0;
}

static void BarcodeKey1_dealloc(BarcodeKey1* self)
{
    LOG("BarcodeKey1 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *BarcodeKey1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    BarcodeKey1 *self;
    self = (BarcodeKey1 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("BarcodeKey1 new\n");
    return (PyObject *)self;
}
static int BarcodeKey1_init(BarcodeKey1 *self, PyObject *args, PyObject *kwds)
{
    LOG("BarcodeKey1 init\n");
    self->priv = do_malloc0(sizeof(barcode_key1_t));
    return 0;
}

static void Barcode_dealloc(Barcode* self)
{
    LOG("Barcode free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *Barcode_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Barcode *self;
    self = (Barcode *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("Barcode new\n");
    return (PyObject *)self;
}
static int Barcode_init(Barcode *self, PyObject *args, PyObject *kwds)
{
    LOG("Barcode init\n");
    self->priv = do_malloc0(sizeof(barcode_rec_t));
    return 0;
}

static void AccountKey0_dealloc(AccountKey0* self)
{
    LOG("AccountKey0 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *AccountKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    AccountKey0 *self;
    self = (AccountKey0 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("AccountKey0 new\n");
    return (PyObject *)self;
}
static int AccountKey0_init(AccountKey0 *self, PyObject *args, PyObject *kwds)
{
    LOG("AccountKey0 init\n");
    self->priv = do_malloc0(sizeof(account_key0_t));
    return 0;
}

static void Account_dealloc(Account* self)
{
    LOG("Account free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *Account_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Account *self;
    self = (Account *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("Account new\n");
    return (PyObject *)self;
}
static int Account_init(Account *self, PyObject *args, PyObject *kwds)
{
    LOG("Account init\n");
    self->priv = do_malloc0(sizeof(account_rec_t));
    return 0;
}

static void SubaccountKey0_dealloc(SubaccountKey0* self)
{
    LOG("SubaccountKey0 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *SubaccountKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    SubaccountKey0 *self;
    self = (SubaccountKey0 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("SubaccountKey0 new\n");
    return (PyObject *)self;
}
static int SubaccountKey0_init(SubaccountKey0 *self, PyObject *args, PyObject *kwds)
{
    LOG("SubaccountKey0 init\n");
    self->priv = do_malloc0(sizeof(subaccount_key0_t));
    return 0;
}

static void Subaccount_dealloc(Subaccount* self)
{
    LOG("Subaccount free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *Subaccount_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Subaccount *self;
    self = (Subaccount *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("Subaccount new\n");
    return (PyObject *)self;
}
static int Subaccount_init(Subaccount *self, PyObject *args, PyObject *kwds)
{
    LOG("Subaccount init\n");
    self->priv = do_malloc0(sizeof(subaccount_rec_t));
    return 0;
}

static void Class_dealloc(Class* self)
{
    LOG("Class free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *Class_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Class *self;
    self = (Class *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("Class new\n");
    return (PyObject *)self;
}
static int Class_init(Class *self, PyObject *args, PyObject *kwds)
{
    LOG("Class init\n");
    self->priv = do_malloc0(sizeof(class_rec_t));
    return 0;
}

static void GroupKey0_dealloc(GroupKey0* self)
{
    LOG("GroupKey0 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *GroupKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    GroupKey0 *self;
    self = (GroupKey0 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("GroupKey0 new\n");
    return (PyObject *)self;
}
static int GroupKey0_init(GroupKey0 *self, PyObject *args, PyObject *kwds)
{
    LOG("GroupKey0 init\n");
    self->priv = do_malloc0(sizeof(group_key0_t));
    return 0;
}

static void GroupKey1_dealloc(GroupKey1* self)
{
    LOG("GroupKey1 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *GroupKey1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    GroupKey1 *self;
    self = (GroupKey1 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("GroupKey1 new\n");
    return (PyObject *)self;
}
static int GroupKey1_init(GroupKey1 *self, PyObject *args, PyObject *kwds)
{
    LOG("GroupKey1 init\n");
    self->priv = do_malloc0(sizeof(group_key1_t));
    return 0;
}

static void Group_dealloc(Group* self)
{
    LOG("Group free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *Group_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Group *self;
    self = (Group *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("Group new\n");
    return (PyObject *)self;
}
static int Group_init(Group *self, PyObject *args, PyObject *kwds)
{
    LOG("Group init\n");
    self->priv = do_malloc0(sizeof(group_rec_t));
    return 0;
}

static void SubgroupKey0_dealloc(SubgroupKey0* self)
{
    LOG("SubgroupKey0 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *SubgroupKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    SubgroupKey0 *self;
    self = (SubgroupKey0 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("SubgroupKey0 new\n");
    return (PyObject *)self;
}
static int SubgroupKey0_init(SubgroupKey0 *self, PyObject *args, PyObject *kwds)
{
    LOG("SubgroupKey0 init\n");
    self->priv = do_malloc0(sizeof(subgroup_key0_t));
    return 0;
}

static void SubgroupKey1_dealloc(SubgroupKey1* self)
{
    LOG("SubgroupKey1 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *SubgroupKey1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    SubgroupKey1 *self;
    self = (SubgroupKey1 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("SubgroupKey1 new\n");
    return (PyObject *)self;
}
static int SubgroupKey1_init(SubgroupKey1 *self, PyObject *args, PyObject *kwds)
{
    LOG("SubgroupKey1 init\n");
    self->priv = do_malloc0(sizeof(subgroup_key1_t));
    return 0;
}

static void Subgroup_dealloc(Subgroup* self)
{
    LOG("Subgroup free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *Subgroup_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Subgroup *self;
    self = (Subgroup *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("Subgroup new\n");
    return (PyObject *)self;
}
static int Subgroup_init(Subgroup *self, PyObject *args, PyObject *kwds)
{
    LOG("Subgroup init\n");
    self->priv = do_malloc0(sizeof(subgroup_rec_t));
    return 0;
}

static void RegionKey0_dealloc(RegionKey0* self)
{
    LOG("RegionKey0 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *RegionKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    RegionKey0 *self;
    self = (RegionKey0 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("RegionKey0 new\n");
    return (PyObject *)self;
}
static int RegionKey0_init(RegionKey0 *self, PyObject *args, PyObject *kwds)
{
    LOG("RegionKey0 init\n");
    self->priv = do_malloc0(sizeof(region_key0_t));
    return 0;
}

static void RegionKey1_dealloc(RegionKey1* self)
{
    LOG("RegionKey1 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *RegionKey1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    RegionKey1 *self;
    self = (RegionKey1 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("RegionKey1 new\n");
    return (PyObject *)self;
}
static int RegionKey1_init(RegionKey1 *self, PyObject *args, PyObject *kwds)
{
    LOG("RegionKey1 init\n");
    self->priv = do_malloc0(sizeof(region_key1_t));
    return 0;
}

static void Region_dealloc(Region* self)
{
    LOG("Region free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *Region_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Region *self;
    self = (Region *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("Region new\n");
    return (PyObject *)self;
}
static int Region_init(Region *self, PyObject *args, PyObject *kwds)
{
    LOG("Region init\n");
    self->priv = do_malloc0(sizeof(region_rec_t));
    return 0;
}

static void OtdelKey0_dealloc(OtdelKey0* self)
{
    LOG("OtdelKey0 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *OtdelKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    OtdelKey0 *self;
    self = (OtdelKey0 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("OtdelKey0 new\n");
    return (PyObject *)self;
}
static int OtdelKey0_init(OtdelKey0 *self, PyObject *args, PyObject *kwds)
{
    LOG("OtdelKey0 init\n");
    self->priv = do_malloc0(sizeof(otdel_key0_t));
    return 0;
}

static void Otdel_dealloc(Otdel* self)
{
    LOG("Otdel free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *Otdel_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Otdel *self;
    self = (Otdel *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("Otdel new\n");
    return (PyObject *)self;
}
static int Otdel_init(Otdel *self, PyObject *args, PyObject *kwds)
{
    LOG("Otdel init\n");
    self->priv = do_malloc0(sizeof(otdel_rec_t));
    return 0;
}

static void PartnerKey0_dealloc(PartnerKey0* self)
{
    LOG("PartnerKey0 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *PartnerKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PartnerKey0 *self;
    self = (PartnerKey0 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("PartnerKey0 new\n");
    return (PyObject *)self;
}
static int PartnerKey0_init(PartnerKey0 *self, PyObject *args, PyObject *kwds)
{
    LOG("PartnerKey0 init\n");
    self->priv = do_malloc0(sizeof(partner_key0_t));
    return 0;
}

static void PartnerKey1_dealloc(PartnerKey1* self)
{
    LOG("PartnerKey1 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *PartnerKey1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PartnerKey1 *self;
    self = (PartnerKey1 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("PartnerKey1 new\n");
    return (PyObject *)self;
}
static int PartnerKey1_init(PartnerKey1 *self, PyObject *args, PyObject *kwds)
{
    LOG("PartnerKey1 init\n");
    self->priv = do_malloc0(sizeof(partner_key1_t));
    return 0;
}

static void PartnerKey2_dealloc(PartnerKey2* self)
{
    LOG("PartnerKey2 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *PartnerKey2_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PartnerKey2 *self;
    self = (PartnerKey2 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("PartnerKey2 new\n");
    return (PyObject *)self;
}
static int PartnerKey2_init(PartnerKey2 *self, PyObject *args, PyObject *kwds)
{
    LOG("PartnerKey2 init\n");
    self->priv = do_malloc0(sizeof(partner_key2_t));
    return 0;
}

static void PartnerKey3_dealloc(PartnerKey3* self)
{
    LOG("PartnerKey3 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *PartnerKey3_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PartnerKey3 *self;
    self = (PartnerKey3 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("PartnerKey3 new\n");
    return (PyObject *)self;
}
static int PartnerKey3_init(PartnerKey3 *self, PyObject *args, PyObject *kwds)
{
    LOG("PartnerKey3 init\n");
    self->priv = do_malloc0(sizeof(partner_key3_t));
    return 0;
}

static void Partner_dealloc(Partner* self)
{
    LOG("Partner free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *Partner_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Partner *self;
    self = (Partner *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("Partner new\n");
    return (PyObject *)self;
}
static int Partner_init(Partner *self, PyObject *args, PyObject *kwds)
{
    LOG("Partner init\n");
    self->priv = do_malloc0(sizeof(partner_rec_t));
    return 0;
}

static void PartnerDataKey0_dealloc(PartnerDataKey0* self)
{
    LOG("PartnerDataKey0 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *PartnerDataKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PartnerDataKey0 *self;
    self = (PartnerDataKey0 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("PartnerDataKey0 new\n");
    return (PyObject *)self;
}
static int PartnerDataKey0_init(PartnerDataKey0 *self, PyObject *args, PyObject *kwds)
{
    LOG("PartnerDataKey0 init\n");
    self->priv = do_malloc0(sizeof(partner_data_key0_t));
    return 0;
}

static void PartnerDataKey1_dealloc(PartnerDataKey1* self)
{
    LOG("PartnerDataKey1 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *PartnerDataKey1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PartnerDataKey1 *self;
    self = (PartnerDataKey1 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("PartnerDataKey1 new\n");
    return (PyObject *)self;
}
static int PartnerDataKey1_init(PartnerDataKey1 *self, PyObject *args, PyObject *kwds)
{
    LOG("PartnerDataKey1 init\n");
    self->priv = do_malloc0(sizeof(partner_data_key1_t));
    return 0;
}

static void PartnerData_dealloc(PartnerData* self)
{
    LOG("PartnerData free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *PartnerData_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PartnerData *self;
    self = (PartnerData *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("PartnerData new\n");
    return (PyObject *)self;
}
static int PartnerData_init(PartnerData *self, PyObject *args, PyObject *kwds)
{
    LOG("PartnerData init\n");
    self->priv = do_malloc0(sizeof(partner_data_rec_t));
    return 0;
}

static void PartnerViewKey0_dealloc(PartnerViewKey0* self)
{
    LOG("PartnerViewKey0 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *PartnerViewKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PartnerViewKey0 *self;
    self = (PartnerViewKey0 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("PartnerViewKey0 new\n");
    return (PyObject *)self;
}
static int PartnerViewKey0_init(PartnerViewKey0 *self, PyObject *args, PyObject *kwds)
{
    LOG("PartnerViewKey0 init\n");
    self->priv = do_malloc0(sizeof(partner_view_key0_t));
    return 0;
}

static void PartnerViewKey1_dealloc(PartnerViewKey1* self)
{
    LOG("PartnerViewKey1 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *PartnerViewKey1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PartnerViewKey1 *self;
    self = (PartnerViewKey1 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("PartnerViewKey1 new\n");
    return (PyObject *)self;
}
static int PartnerViewKey1_init(PartnerViewKey1 *self, PyObject *args, PyObject *kwds)
{
    LOG("PartnerViewKey1 init\n");
    self->priv = do_malloc0(sizeof(partner_view_key1_t));
    return 0;
}

static void PartnerView_dealloc(PartnerView* self)
{
    LOG("PartnerView free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *PartnerView_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PartnerView *self;
    self = (PartnerView *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("PartnerView new\n");
    return (PyObject *)self;
}
static int PartnerView_init(PartnerView *self, PyObject *args, PyObject *kwds)
{
    LOG("PartnerView init\n");
    self->priv = do_malloc0(sizeof(partner_view_rec_t));
    return 0;
}

static void SkladKey0_dealloc(SkladKey0* self)
{
    LOG("SkladKey0 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *SkladKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    SkladKey0 *self;
    self = (SkladKey0 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("SkladKey0 new\n");
    return (PyObject *)self;
}
static int SkladKey0_init(SkladKey0 *self, PyObject *args, PyObject *kwds)
{
    LOG("SkladKey0 init\n");
    self->priv = do_malloc0(sizeof(sklad_key0_t));
    return 0;
}

static void Sklad_dealloc(Sklad* self)
{
    LOG("Sklad free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *Sklad_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Sklad *self;
    self = (Sklad *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("Sklad new\n");
    return (PyObject *)self;
}
static int Sklad_init(Sklad *self, PyObject *args, PyObject *kwds)
{
    LOG("Sklad init\n");
    self->priv = do_malloc0(sizeof(sklad_rec_t));
    return 0;
}

static void DocumentProwKey0_dealloc(DocumentProwKey0* self)
{
    LOG("DocumentProwKey0 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *DocumentProwKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    DocumentProwKey0 *self;
    self = (DocumentProwKey0 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("DocumentProwKey0 new\n");
    return (PyObject *)self;
}
static int DocumentProwKey0_init(DocumentProwKey0 *self, PyObject *args, PyObject *kwds)
{
    LOG("DocumentProwKey0 init\n");
    self->priv = do_malloc0(sizeof(document_prow_key0_t));
    return 0;
}

static void DocumentProw_dealloc(DocumentProw* self)
{
    LOG("DocumentProw free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *DocumentProw_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    DocumentProw *self;
    self = (DocumentProw *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("DocumentProw new\n");
    return (PyObject *)self;
}
static int DocumentProw_init(DocumentProw *self, PyObject *args, PyObject *kwds)
{
    LOG("DocumentProw init\n");
    self->priv = do_malloc0(sizeof(document_prow_rec_t));
    return 0;
}

static void DocumentTypeKey0_dealloc(DocumentTypeKey0* self)
{
    LOG("DocumentTypeKey0 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *DocumentTypeKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    DocumentTypeKey0 *self;
    self = (DocumentTypeKey0 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("DocumentTypeKey0 new\n");
    return (PyObject *)self;
}
static int DocumentTypeKey0_init(DocumentTypeKey0 *self, PyObject *args, PyObject *kwds)
{
    LOG("DocumentTypeKey0 init\n");
    self->priv = do_malloc0(sizeof(document_type_key0_t));
    return 0;
}

static void DocumentType_dealloc(DocumentType* self)
{
    LOG("DocumentType free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *DocumentType_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    DocumentType *self;
    self = (DocumentType *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("DocumentType new\n");
    return (PyObject *)self;
}
static int DocumentType_init(DocumentType *self, PyObject *args, PyObject *kwds)
{
    LOG("DocumentType init\n");
    self->priv = do_malloc0(sizeof(document_type_rec_t));
    return 0;
}

static void StockKey0_dealloc(StockKey0* self)
{
    LOG("StockKey0 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *StockKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    StockKey0 *self;
    self = (StockKey0 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("StockKey0 new\n");
    return (PyObject *)self;
}
static int StockKey0_init(StockKey0 *self, PyObject *args, PyObject *kwds)
{
    LOG("StockKey0 init\n");
    self->priv = do_malloc0(sizeof(stock_key0_t));
    return 0;
}

static void StockKey1_dealloc(StockKey1* self)
{
    LOG("StockKey1 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *StockKey1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    StockKey1 *self;
    self = (StockKey1 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("StockKey1 new\n");
    return (PyObject *)self;
}
static int StockKey1_init(StockKey1 *self, PyObject *args, PyObject *kwds)
{
    LOG("StockKey1 init\n");
    self->priv = do_malloc0(sizeof(stock_key1_t));
    return 0;
}

static void Stock_dealloc(Stock* self)
{
    LOG("Stock free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *Stock_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Stock *self;
    self = (Stock *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("Stock new\n");
    return (PyObject *)self;
}
static int Stock_init(Stock *self, PyObject *args, PyObject *kwds)
{
    LOG("Stock init\n");
    self->priv = do_malloc0(sizeof(stock_rec_t));
    return 0;
}

static void ProwodKey0_dealloc(ProwodKey0* self)
{
    LOG("ProwodKey0 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *ProwodKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    ProwodKey0 *self;
    self = (ProwodKey0 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("ProwodKey0 new\n");
    return (PyObject *)self;
}
static int ProwodKey0_init(ProwodKey0 *self, PyObject *args, PyObject *kwds)
{
    LOG("ProwodKey0 init\n");
    self->priv = do_malloc0(sizeof(prowod_key0_t));
    return 0;
}

static void ProwodKey1_dealloc(ProwodKey1* self)
{
    LOG("ProwodKey1 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *ProwodKey1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    ProwodKey1 *self;
    self = (ProwodKey1 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("ProwodKey1 new\n");
    return (PyObject *)self;
}
static int ProwodKey1_init(ProwodKey1 *self, PyObject *args, PyObject *kwds)
{
    LOG("ProwodKey1 init\n");
    self->priv = do_malloc0(sizeof(prowod_key1_t));
    return 0;
}

static void ProwodKey2_dealloc(ProwodKey2* self)
{
    LOG("ProwodKey2 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *ProwodKey2_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    ProwodKey2 *self;
    self = (ProwodKey2 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("ProwodKey2 new\n");
    return (PyObject *)self;
}
static int ProwodKey2_init(ProwodKey2 *self, PyObject *args, PyObject *kwds)
{
    LOG("ProwodKey2 init\n");
    self->priv = do_malloc0(sizeof(prowod_key2_t));
    return 0;
}

static void ProwodKey3_dealloc(ProwodKey3* self)
{
    LOG("ProwodKey3 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *ProwodKey3_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    ProwodKey3 *self;
    self = (ProwodKey3 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("ProwodKey3 new\n");
    return (PyObject *)self;
}
static int ProwodKey3_init(ProwodKey3 *self, PyObject *args, PyObject *kwds)
{
    LOG("ProwodKey3 init\n");
    self->priv = do_malloc0(sizeof(prowod_key3_t));
    return 0;
}

static void ProwodKey4_dealloc(ProwodKey4* self)
{
    LOG("ProwodKey4 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *ProwodKey4_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    ProwodKey4 *self;
    self = (ProwodKey4 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("ProwodKey4 new\n");
    return (PyObject *)self;
}
static int ProwodKey4_init(ProwodKey4 *self, PyObject *args, PyObject *kwds)
{
    LOG("ProwodKey4 init\n");
    self->priv = do_malloc0(sizeof(prowod_key4_t));
    return 0;
}

static void Prowod_dealloc(Prowod* self)
{
    LOG("Prowod free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *Prowod_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Prowod *self;
    self = (Prowod *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("Prowod new\n");
    return (PyObject *)self;
}
static int Prowod_init(Prowod *self, PyObject *args, PyObject *kwds)
{
    LOG("Prowod init\n");
    self->priv = do_malloc0(sizeof(prowod_rec_t));
    return 0;
}

static void Prowod2Key0_dealloc(Prowod2Key0* self)
{
    LOG("Prowod2Key0 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *Prowod2Key0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Prowod2Key0 *self;
    self = (Prowod2Key0 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("Prowod2Key0 new\n");
    return (PyObject *)self;
}
static int Prowod2Key0_init(Prowod2Key0 *self, PyObject *args, PyObject *kwds)
{
    LOG("Prowod2Key0 init\n");
    self->priv = do_malloc0(sizeof(prowod2_key0_t));
    return 0;
}

static void Prowod2Key1_dealloc(Prowod2Key1* self)
{
    LOG("Prowod2Key1 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *Prowod2Key1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Prowod2Key1 *self;
    self = (Prowod2Key1 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("Prowod2Key1 new\n");
    return (PyObject *)self;
}
static int Prowod2Key1_init(Prowod2Key1 *self, PyObject *args, PyObject *kwds)
{
    LOG("Prowod2Key1 init\n");
    self->priv = do_malloc0(sizeof(prowod2_key1_t));
    return 0;
}

static void Prowod2Key2_dealloc(Prowod2Key2* self)
{
    LOG("Prowod2Key2 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *Prowod2Key2_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Prowod2Key2 *self;
    self = (Prowod2Key2 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("Prowod2Key2 new\n");
    return (PyObject *)self;
}
static int Prowod2Key2_init(Prowod2Key2 *self, PyObject *args, PyObject *kwds)
{
    LOG("Prowod2Key2 init\n");
    self->priv = do_malloc0(sizeof(prowod2_key2_t));
    return 0;
}

static void Prowod2Key3_dealloc(Prowod2Key3* self)
{
    LOG("Prowod2Key3 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *Prowod2Key3_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Prowod2Key3 *self;
    self = (Prowod2Key3 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("Prowod2Key3 new\n");
    return (PyObject *)self;
}
static int Prowod2Key3_init(Prowod2Key3 *self, PyObject *args, PyObject *kwds)
{
    LOG("Prowod2Key3 init\n");
    self->priv = do_malloc0(sizeof(prowod2_key3_t));
    return 0;
}

static void Prowod2_dealloc(Prowod2* self)
{
    LOG("Prowod2 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *Prowod2_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Prowod2 *self;
    self = (Prowod2 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("Prowod2 new\n");
    return (PyObject *)self;
}
static int Prowod2_init(Prowod2 *self, PyObject *args, PyObject *kwds)
{
    LOG("Prowod2 init\n");
    self->priv = do_malloc0(sizeof(prowod2_rec_t));
    return 0;
}

static void SaldoKey0_dealloc(SaldoKey0* self)
{
    LOG("SaldoKey0 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *SaldoKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    SaldoKey0 *self;
    self = (SaldoKey0 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("SaldoKey0 new\n");
    return (PyObject *)self;
}
static int SaldoKey0_init(SaldoKey0 *self, PyObject *args, PyObject *kwds)
{
    LOG("SaldoKey0 init\n");
    self->priv = do_malloc0(sizeof(saldo_key0_t));
    return 0;
}

static void Saldo_dealloc(Saldo* self)
{
    LOG("Saldo free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *Saldo_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Saldo *self;
    self = (Saldo *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("Saldo new\n");
    return (PyObject *)self;
}
static int Saldo_init(Saldo *self, PyObject *args, PyObject *kwds)
{
    LOG("Saldo init\n");
    self->priv = do_malloc0(sizeof(saldo_rec_t));
    return 0;
}

static void ProtocolKey0_dealloc(ProtocolKey0* self)
{
    LOG("ProtocolKey0 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *ProtocolKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    ProtocolKey0 *self;
    self = (ProtocolKey0 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("ProtocolKey0 new\n");
    return (PyObject *)self;
}
static int ProtocolKey0_init(ProtocolKey0 *self, PyObject *args, PyObject *kwds)
{
    LOG("ProtocolKey0 init\n");
    self->priv = do_malloc0(sizeof(protocol_key0_t));
    return 0;
}

static void ProtocolKey1_dealloc(ProtocolKey1* self)
{
    LOG("ProtocolKey1 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *ProtocolKey1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    ProtocolKey1 *self;
    self = (ProtocolKey1 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("ProtocolKey1 new\n");
    return (PyObject *)self;
}
static int ProtocolKey1_init(ProtocolKey1 *self, PyObject *args, PyObject *kwds)
{
    LOG("ProtocolKey1 init\n");
    self->priv = do_malloc0(sizeof(protocol_key1_t));
    return 0;
}

static void ProtocolKey2_dealloc(ProtocolKey2* self)
{
    LOG("ProtocolKey2 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *ProtocolKey2_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    ProtocolKey2 *self;
    self = (ProtocolKey2 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("ProtocolKey2 new\n");
    return (PyObject *)self;
}
static int ProtocolKey2_init(ProtocolKey2 *self, PyObject *args, PyObject *kwds)
{
    LOG("ProtocolKey2 init\n");
    self->priv = do_malloc0(sizeof(protocol_key2_t));
    return 0;
}

static void Protocol_dealloc(Protocol* self)
{
    LOG("Protocol free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *Protocol_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Protocol *self;
    self = (Protocol *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("Protocol new\n");
    return (PyObject *)self;
}
static int Protocol_init(Protocol *self, PyObject *args, PyObject *kwds)
{
    LOG("Protocol init\n");
    self->priv = do_malloc0(sizeof(protocol_rec_t));
    return 0;
}

static void UserKey0_dealloc(UserKey0* self)
{
    LOG("UserKey0 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *UserKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    UserKey0 *self;
    self = (UserKey0 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("UserKey0 new\n");
    return (PyObject *)self;
}
static int UserKey0_init(UserKey0 *self, PyObject *args, PyObject *kwds)
{
    LOG("UserKey0 init\n");
    self->priv = do_malloc0(sizeof(user_key0_t));
    return 0;
}

static void User_dealloc(User* self)
{
    LOG("User free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *User_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    User *self;
    self = (User *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("User new\n");
    return (PyObject *)self;
}
static int User_init(User *self, PyObject *args, PyObject *kwds)
{
    LOG("User init\n");
    self->priv = do_malloc0(sizeof(user_rec_t));
    return 0;
}

static void ShiftKey0_dealloc(ShiftKey0* self)
{
    LOG("ShiftKey0 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *ShiftKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    ShiftKey0 *self;
    self = (ShiftKey0 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("ShiftKey0 new\n");
    return (PyObject *)self;
}
static int ShiftKey0_init(ShiftKey0 *self, PyObject *args, PyObject *kwds)
{
    LOG("ShiftKey0 init\n");
    self->priv = do_malloc0(sizeof(shift_key0_t));
    return 0;
}

static void ShiftKey1_dealloc(ShiftKey1* self)
{
    LOG("ShiftKey1 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *ShiftKey1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    ShiftKey1 *self;
    self = (ShiftKey1 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("ShiftKey1 new\n");
    return (PyObject *)self;
}
static int ShiftKey1_init(ShiftKey1 *self, PyObject *args, PyObject *kwds)
{
    LOG("ShiftKey1 init\n");
    self->priv = do_malloc0(sizeof(shift_key1_t));
    return 0;
}

static void ShiftKey2_dealloc(ShiftKey2* self)
{
    LOG("ShiftKey2 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *ShiftKey2_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    ShiftKey2 *self;
    self = (ShiftKey2 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("ShiftKey2 new\n");
    return (PyObject *)self;
}
static int ShiftKey2_init(ShiftKey2 *self, PyObject *args, PyObject *kwds)
{
    LOG("ShiftKey2 init\n");
    self->priv = do_malloc0(sizeof(shift_key2_t));
    return 0;
}

static void ShiftKey3_dealloc(ShiftKey3* self)
{
    LOG("ShiftKey3 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *ShiftKey3_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    ShiftKey3 *self;
    self = (ShiftKey3 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("ShiftKey3 new\n");
    return (PyObject *)self;
}
static int ShiftKey3_init(ShiftKey3 *self, PyObject *args, PyObject *kwds)
{
    LOG("ShiftKey3 init\n");
    self->priv = do_malloc0(sizeof(shift_key3_t));
    return 0;
}

static void Shift_dealloc(Shift* self)
{
    LOG("Shift free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *Shift_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Shift *self;
    self = (Shift *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("Shift new\n");
    return (PyObject *)self;
}
static int Shift_init(Shift *self, PyObject *args, PyObject *kwds)
{
    LOG("Shift init\n");
    self->priv = do_malloc0(sizeof(shift_rec_t));
    return 0;
}
static void ChecksumKey0_dealloc(ChecksumKey0* self)
{
    LOG("ChecksumKey0 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *ChecksumKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    ChecksumKey0 *self;
    self = (ChecksumKey0 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("ChecksumKey0 new\n");
    return (PyObject *)self;
}
static int ChecksumKey0_init(ChecksumKey0 *self, PyObject *args, PyObject *kwds)
{
    LOG("ChecksumKey0 init\n");
    self->priv = do_malloc0(sizeof(checksum_key0_t));
    return 0;
}

static void ChecksumKey1_dealloc(ChecksumKey1* self)
{
    LOG("ChecksumKey1 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *ChecksumKey1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    ChecksumKey1 *self;
    self = (ChecksumKey1 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("ChecksumKey1 new\n");
    return (PyObject *)self;
}
static int ChecksumKey1_init(ChecksumKey1 *self, PyObject *args, PyObject *kwds)
{
    LOG("ChecksumKey1 init\n");
    self->priv = do_malloc0(sizeof(checksum_key1_t));
    return 0;
}

static void Checksum_dealloc(Checksum* self)
{
    LOG("Checksum free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *Checksum_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Checksum *self;
    self = (Checksum *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("Checksum new\n");
    return (PyObject *)self;
}
static int Checksum_init(Checksum *self, PyObject *args, PyObject *kwds)
{
    LOG("Checksum init\n");
    self->priv = do_malloc0(sizeof(checksum_rec_t));
    return 0;
}

static void CheckKey0_dealloc(CheckKey0* self)
{
    LOG("CheckKey0 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *CheckKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    CheckKey0 *self;
    self = (CheckKey0 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("CheckKey0 new\n");
    return (PyObject *)self;
}
static int CheckKey0_init(CheckKey0 *self, PyObject *args, PyObject *kwds)
{
    LOG("CheckKey0 init\n");
    self->priv = do_malloc0(sizeof(check_key0_t));
    return 0;
}

static void CheckKey1_dealloc(CheckKey1* self)
{
    LOG("CheckKey1 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *CheckKey1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    CheckKey1 *self;
    self = (CheckKey1 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("CheckKey1 new\n");
    return (PyObject *)self;
}
static int CheckKey1_init(CheckKey1 *self, PyObject *args, PyObject *kwds)
{
    LOG("CheckKey1 init\n");
    self->priv = do_malloc0(sizeof(check_key1_t));
    return 0;
}

static void CheckKey2_dealloc(CheckKey2* self)
{
    LOG("CheckKey2 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *CheckKey2_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    CheckKey2 *self;
    self = (CheckKey2 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("CheckKey2 new\n");
    return (PyObject *)self;
}
static int CheckKey2_init(CheckKey2 *self, PyObject *args, PyObject *kwds)
{
    LOG("CheckKey2 init\n");
    self->priv = do_malloc0(sizeof(check_key2_t));
    return 0;
}

static void Check_dealloc(Check* self)
{
    LOG("Check free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *Check_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Check *self;
    self = (Check *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("Check new\n");
    return (PyObject *)self;
}
static int Check_init(Check *self, PyObject *args, PyObject *kwds)
{
    LOG("Check init\n");
    self->priv = do_malloc0(sizeof(check_rec_t));
    return 0;
}

static void RealizationKey0_dealloc(RealizationKey0* self)
{
    LOG("RealizationKey0 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *RealizationKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    RealizationKey0 *self;
    self = (RealizationKey0 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("RealizationKey0 new\n");
    return (PyObject *)self;
}
static int RealizationKey0_init(RealizationKey0 *self, PyObject *args, PyObject *kwds)
{
    LOG("RealizationKey0 init\n");
    self->priv = do_malloc0(sizeof(realization_key0_t));
    return 0;
}

static void RealizationKey1_dealloc(RealizationKey1* self)
{
    LOG("RealizationKey1 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *RealizationKey1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    RealizationKey1 *self;
    self = (RealizationKey1 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("RealizationKey1 new\n");
    return (PyObject *)self;
}
static int RealizationKey1_init(RealizationKey1 *self, PyObject *args, PyObject *kwds)
{
    LOG("RealizationKey1 init\n");
    self->priv = do_malloc0(sizeof(realization_key1_t));
    return 0;
}

static void RealizationKey2_dealloc(RealizationKey2* self)
{
    LOG("RealizationKey2 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *RealizationKey2_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    RealizationKey2 *self;
    self = (RealizationKey2 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("RealizationKey2 new\n");
    return (PyObject *)self;
}
static int RealizationKey2_init(RealizationKey2 *self, PyObject *args, PyObject *kwds)
{
    LOG("RealizationKey2 init\n");
    self->priv = do_malloc0(sizeof(realization_key2_t));
    return 0;
}

static void RealizationKey3_dealloc(RealizationKey3* self)
{
    LOG("RealizationKey3 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *RealizationKey3_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    RealizationKey3 *self;
    self = (RealizationKey3 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("RealizationKey3 new\n");
    return (PyObject *)self;
}
static int RealizationKey3_init(RealizationKey3 *self, PyObject *args, PyObject *kwds)
{
    LOG("RealizationKey3 init\n");
    self->priv = do_malloc0(sizeof(realization_key3_t));
    return 0;
}

static void RealizationKey4_dealloc(RealizationKey4* self)
{
    LOG("RealizationKey4 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *RealizationKey4_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    RealizationKey4 *self;
    self = (RealizationKey4 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("RealizationKey4 new\n");
    return (PyObject *)self;
}
static int RealizationKey4_init(RealizationKey4 *self, PyObject *args, PyObject *kwds)
{
    LOG("RealizationKey4 init\n");
    self->priv = do_malloc0(sizeof(realization_key4_t));
    return 0;
}

static void Realization_dealloc(Realization* self)
{
    LOG("Realization free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *Realization_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Realization *self;
    self = (Realization *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("Realization new\n");
    return (PyObject *)self;
}
static int Realization_init(Realization *self, PyObject *args, PyObject *kwds)
{
    LOG("Realization init\n");
    self->priv = do_malloc0(sizeof(realization_rec_t));
    return 0;
}

static void DiscountKey0_dealloc(DiscountKey0* self)
{
    LOG("DiscountKey0 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *DiscountKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    DiscountKey0 *self;
    self = (DiscountKey0 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("DiscountKey0 new\n");
    return (PyObject *)self;
}
static int DiscountKey0_init(DiscountKey0 *self, PyObject *args, PyObject *kwds)
{
    LOG("DiscountKey0 init\n");
    self->priv = do_malloc0(sizeof(discount_key0_t));
    return 0;
}

static void DiscountKey1_dealloc(DiscountKey1* self)
{
    LOG("DiscountKey1 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *DiscountKey1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    DiscountKey1 *self;
    self = (DiscountKey1 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("DiscountKey1 new\n");
    return (PyObject *)self;
}
static int DiscountKey1_init(DiscountKey1 *self, PyObject *args, PyObject *kwds)
{
    LOG("DiscountKey1 init\n");
    self->priv = do_malloc0(sizeof(discount_key1_t));
    return 0;
}

static void Discount_dealloc(Discount* self)
{
    LOG("Discount free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *Discount_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Discount *self;
    self = (Discount *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("Discount new\n");
    return (PyObject *)self;
}
static int Discount_init(Discount *self, PyObject *args, PyObject *kwds)
{
    LOG("Discount init\n");
    self->priv = do_malloc0(sizeof(discount_rec_t));
    return 0;
}

static void StructFileKey0_dealloc(StructFileKey0* self)
{
    LOG("StructFileKey0 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *StructFileKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    StructFileKey0 *self;
    self = (StructFileKey0 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("StructFileKey0 new\n");
    return (PyObject *)self;
}
static int StructFileKey0_init(StructFileKey0 *self, PyObject *args, PyObject *kwds)
{
    LOG("StructFileKey0 init\n");
    self->priv = do_malloc0(sizeof(struct_file_key0_t));
    return 0;
}

static void StructFileKey1_dealloc(StructFileKey1* self)
{
    LOG("StructFileKey1 free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *StructFileKey1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    StructFileKey1 *self;
    self = (StructFileKey1 *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("StructFileKey1 new\n");
    return (PyObject *)self;
}
static int StructFileKey1_init(StructFileKey1 *self, PyObject *args, PyObject *kwds)
{
    LOG("StructFileKey1 init\n");
    self->priv = do_malloc0(sizeof(struct_file_key1_t));
    return 0;
}

static void StructFile_dealloc(StructFile* self)
{
    LOG("StructFile free\n");
    do_free(self->priv);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *StructFile_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    StructFile *self;
    self = (StructFile *)type->tp_alloc(type, 0);
    if ( self != NULL )
        self->priv = NULL;
    LOG("StructFile new\n");
    return (PyObject *)self;
}
static int StructFile_init(StructFile *self, PyObject *args, PyObject *kwds)
{
    LOG("StructFile init\n");
    self->priv = do_malloc0(sizeof(struct_file_rec_t));
    return 0;
}
//!!!!!

static PyMethodDef Document_methods[] = {
	{"get_dtype", (PyCFunction)Document_get_dtype, METH_NOARGS,"do_alias_get_dtype"},
	{"set_dtype", (PyCFunction)Document_set_dtype, METH_VARARGS|METH_KEYWORDS,"do_alias_get_dtype"},
	{"get_dtype1", (PyCFunction)Document_get_dtype1, METH_NOARGS,"do_alias_get_dtype1"},
	{"set_dtype1", (PyCFunction)Document_set_dtype1, METH_VARARGS|METH_KEYWORDS,"do_alias_get_dtype1"},
	{"get_name", (PyCFunction)Document_get_name, METH_NOARGS,"do_alias_get_name"},
	{"set_name", (PyCFunction)Document_set_name, METH_VARARGS|METH_KEYWORDS,"do_alias_get_name"},
	{"get_sklad", (PyCFunction)Document_get_sklad, METH_NOARGS,"do_alias_get_sklad"},
	{"set_sklad", (PyCFunction)Document_set_sklad, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sklad"},
	{"get_document", (PyCFunction)Document_get_document, METH_NOARGS,"do_alias_get_document"},
	{"set_document", (PyCFunction)Document_set_document, METH_VARARGS|METH_KEYWORDS,"do_alias_get_document"},
	{"get_date", (PyCFunction)Document_get_date, METH_NOARGS,"do_alias_get_date"},
	{"set_date", (PyCFunction)Document_set_date, METH_VARARGS|METH_KEYWORDS,"do_alias_get_date"},
	{"get_time", (PyCFunction)Document_get_time, METH_NOARGS,"do_alias_get_time"},
	{"set_time", (PyCFunction)Document_set_time, METH_VARARGS|METH_KEYWORDS,"do_alias_get_time"},
	{"get_p_g_code", (PyCFunction)Document_get_p_g_code, METH_NOARGS,"do_alias_get_p_g_code"},
	{"set_p_g_code", (PyCFunction)Document_set_p_g_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_p_g_code"},
	{"get_p_code", (PyCFunction)Document_get_p_code, METH_NOARGS,"do_alias_get_p_code"},
	{"set_p_code", (PyCFunction)Document_set_p_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_p_code"},
	{"get_p_doc", (PyCFunction)Document_get_p_doc, METH_NOARGS,"do_alias_get_p_doc"},
	{"set_p_doc", (PyCFunction)Document_set_p_doc, METH_VARARGS|METH_KEYWORDS,"do_alias_get_p_doc"},
	{"get_p_date", (PyCFunction)Document_get_p_date, METH_NOARGS,"do_alias_get_p_date"},
	{"set_p_date", (PyCFunction)Document_set_p_date, METH_VARARGS|METH_KEYWORDS,"do_alias_get_p_date"},
	{"get_p_time", (PyCFunction)Document_get_p_time, METH_NOARGS,"do_alias_get_p_time"},
	{"set_p_time", (PyCFunction)Document_set_p_time, METH_VARARGS|METH_KEYWORDS,"do_alias_get_p_time"},
	{"get_party", (PyCFunction)Document_get_party, METH_NOARGS,"do_alias_get_party"},
	{"set_party", (PyCFunction)Document_set_party, METH_VARARGS|METH_KEYWORDS,"do_alias_get_party"},
	{"get_accepted", (PyCFunction)Document_get_accepted, METH_NOARGS,"do_alias_get_accepted"},
	{"set_accepted", (PyCFunction)Document_set_accepted, METH_VARARGS|METH_KEYWORDS,"do_alias_get_accepted"},
	{"get_params", (PyCFunction)Document_get_params, METH_NOARGS,"do_alias_get_params"},
	{"set_params", (PyCFunction)Document_set_params, METH_VARARGS|METH_KEYWORDS,"do_alias_get_params"},

 {NULL}  /* Sentinel */
};

static PyMethodDef DocumentKey0_methods[] = {
	{"get_dtype", (PyCFunction)DocumentKey0_get_dtype, METH_NOARGS,"do_alias_get_dtype"},
	{"set_dtype", (PyCFunction)DocumentKey0_set_dtype, METH_VARARGS|METH_KEYWORDS,"do_alias_get_dtype"},
	{"get_sklad", (PyCFunction)DocumentKey0_get_sklad, METH_NOARGS,"do_alias_get_sklad"},
	{"set_sklad", (PyCFunction)DocumentKey0_set_sklad, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sklad"},
	{"get_document", (PyCFunction)DocumentKey0_get_document, METH_NOARGS,"do_alias_get_document"},
	{"set_document", (PyCFunction)DocumentKey0_set_document, METH_VARARGS|METH_KEYWORDS,"do_alias_get_document"},

 {NULL}  /* Sentinel */
};

static PyMethodDef DocumentKey1_methods[] = {
	{"get_dtype", (PyCFunction)DocumentKey1_get_dtype, METH_NOARGS,"do_alias_get_dtype"},
	{"set_dtype", (PyCFunction)DocumentKey1_set_dtype, METH_VARARGS|METH_KEYWORDS,"do_alias_get_dtype"},
	{"get_date", (PyCFunction)DocumentKey1_get_date, METH_NOARGS,"do_alias_get_date"},
	{"set_date", (PyCFunction)DocumentKey1_set_date, METH_VARARGS|METH_KEYWORDS,"do_alias_get_date"},
	{"get_time", (PyCFunction)DocumentKey1_get_time, METH_NOARGS,"do_alias_get_time"},
	{"set_time", (PyCFunction)DocumentKey1_set_time, METH_VARARGS|METH_KEYWORDS,"do_alias_get_time"},

 {NULL}  /* Sentinel */
};

static PyMethodDef DocumentKey2_methods[] = {
	{"get_p_g_code", (PyCFunction)DocumentKey2_get_p_g_code, METH_NOARGS,"do_alias_get_p_g_code"},
	{"set_p_g_code", (PyCFunction)DocumentKey2_set_p_g_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_p_g_code"},
	{"get_p_code", (PyCFunction)DocumentKey2_get_p_code, METH_NOARGS,"do_alias_get_p_code"},
	{"set_p_code", (PyCFunction)DocumentKey2_set_p_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_p_code"},
	{"get_party", (PyCFunction)DocumentKey2_get_party, METH_NOARGS,"do_alias_get_party"},
	{"set_party", (PyCFunction)DocumentKey2_set_party, METH_VARARGS|METH_KEYWORDS,"do_alias_get_party"},
	{"get_date", (PyCFunction)DocumentKey2_get_date, METH_NOARGS,"do_alias_get_date"},
	{"set_date", (PyCFunction)DocumentKey2_set_date, METH_VARARGS|METH_KEYWORDS,"do_alias_get_date"},
	{"get_time", (PyCFunction)DocumentKey2_get_time, METH_NOARGS,"do_alias_get_time"},
	{"set_time", (PyCFunction)DocumentKey2_set_time, METH_VARARGS|METH_KEYWORDS,"do_alias_get_time"},

 {NULL}  /* Sentinel */
};

static PyMethodDef DocumentKey3_methods[] = {
	{"get_dtype", (PyCFunction)DocumentKey3_get_dtype, METH_NOARGS,"do_alias_get_dtype"},
	{"set_dtype", (PyCFunction)DocumentKey3_set_dtype, METH_VARARGS|METH_KEYWORDS,"do_alias_get_dtype"},
	{"get_sklad", (PyCFunction)DocumentKey3_get_sklad, METH_NOARGS,"do_alias_get_sklad"},
	{"set_sklad", (PyCFunction)DocumentKey3_set_sklad, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sklad"},
	{"get_date", (PyCFunction)DocumentKey3_get_date, METH_NOARGS,"do_alias_get_date"},
	{"set_date", (PyCFunction)DocumentKey3_set_date, METH_VARARGS|METH_KEYWORDS,"do_alias_get_date"},
	{"get_time", (PyCFunction)DocumentKey3_get_time, METH_NOARGS,"do_alias_get_time"},
	{"set_time", (PyCFunction)DocumentKey3_set_time, METH_VARARGS|METH_KEYWORDS,"do_alias_get_time"},

 {NULL}  /* Sentinel */
};

static PyMethodDef DocumentOrder_methods[] = {
	{"get_dtype", (PyCFunction)DocumentOrder_get_dtype, METH_NOARGS,"do_alias_get_dtype"},
	{"set_dtype", (PyCFunction)DocumentOrder_set_dtype, METH_VARARGS|METH_KEYWORDS,"do_alias_get_dtype"},
	{"get_dtype1", (PyCFunction)DocumentOrder_get_dtype1, METH_NOARGS,"do_alias_get_dtype1"},
	{"set_dtype1", (PyCFunction)DocumentOrder_set_dtype1, METH_VARARGS|METH_KEYWORDS,"do_alias_get_dtype1"},
	{"get_sklad", (PyCFunction)DocumentOrder_get_sklad, METH_NOARGS,"do_alias_get_sklad"},
	{"set_sklad", (PyCFunction)DocumentOrder_set_sklad, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sklad"},
	{"get_document", (PyCFunction)DocumentOrder_get_document, METH_NOARGS,"do_alias_get_document"},
	{"set_document", (PyCFunction)DocumentOrder_set_document, METH_VARARGS|METH_KEYWORDS,"do_alias_get_document"},
	{"get_line", (PyCFunction)DocumentOrder_get_line, METH_NOARGS,"do_alias_get_line"},
	{"set_line", (PyCFunction)DocumentOrder_set_line, METH_VARARGS|METH_KEYWORDS,"do_alias_get_line"},
	{"get_sort", (PyCFunction)DocumentOrder_get_sort, METH_NOARGS,"do_alias_get_sort"},
	{"set_sort", (PyCFunction)DocumentOrder_set_sort, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sort"},
	{"get_code", (PyCFunction)DocumentOrder_get_code, METH_NOARGS,"do_alias_get_code"},
	{"set_code", (PyCFunction)DocumentOrder_set_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_code"},
	{"get_date", (PyCFunction)DocumentOrder_get_date, METH_NOARGS,"do_alias_get_date"},
	{"set_date", (PyCFunction)DocumentOrder_set_date, METH_VARARGS|METH_KEYWORDS,"do_alias_get_date"},
	{"get_time", (PyCFunction)DocumentOrder_get_time, METH_NOARGS,"do_alias_get_time"},
	{"set_time", (PyCFunction)DocumentOrder_set_time, METH_VARARGS|METH_KEYWORDS,"do_alias_get_time"},
	{"get_quant", (PyCFunction)DocumentOrder_get_quant, METH_NOARGS,"do_alias_get_quant"},
	{"set_quant", (PyCFunction)DocumentOrder_set_quant, METH_VARARGS|METH_KEYWORDS,"do_alias_get_quant"},
	{"get_nquant", (PyCFunction)DocumentOrder_get_nquant, METH_NOARGS,"do_alias_get_nquant"},
	{"set_nquant", (PyCFunction)DocumentOrder_set_nquant, METH_VARARGS|METH_KEYWORDS,"do_alias_get_nquant"},
	{"get_price", (PyCFunction)DocumentOrder_get_price, METH_NOARGS,"do_alias_get_price"},
	{"set_price", (PyCFunction)DocumentOrder_set_price, METH_VARARGS|METH_KEYWORDS,"do_alias_get_price"},
	{"get_nprice", (PyCFunction)DocumentOrder_get_nprice, METH_NOARGS,"do_alias_get_nprice"},
	{"set_nprice", (PyCFunction)DocumentOrder_set_nprice, METH_VARARGS|METH_KEYWORDS,"do_alias_get_nprice"},
	{"get_accepted", (PyCFunction)DocumentOrder_get_accepted, METH_NOARGS,"do_alias_get_accepted"},
	{"set_accepted", (PyCFunction)DocumentOrder_set_accepted, METH_VARARGS|METH_KEYWORDS,"do_alias_get_accepted"},
	{"get_params", (PyCFunction)DocumentOrder_get_params, METH_NOARGS,"do_alias_get_params"},
	{"set_params", (PyCFunction)DocumentOrder_set_params, METH_VARARGS|METH_KEYWORDS,"do_alias_get_params"},

 {NULL}  /* Sentinel */
};

static PyMethodDef DocumentOrderKey0_methods[] = {
	{"get_dtype", (PyCFunction)DocumentOrderKey0_get_dtype, METH_NOARGS,"do_alias_get_dtype"},
	{"set_dtype", (PyCFunction)DocumentOrderKey0_set_dtype, METH_VARARGS|METH_KEYWORDS,"do_alias_get_dtype"},
	{"get_sklad", (PyCFunction)DocumentOrderKey0_get_sklad, METH_NOARGS,"do_alias_get_sklad"},
	{"set_sklad", (PyCFunction)DocumentOrderKey0_set_sklad, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sklad"},
	{"get_document", (PyCFunction)DocumentOrderKey0_get_document, METH_NOARGS,"do_alias_get_document"},
	{"set_document", (PyCFunction)DocumentOrderKey0_set_document, METH_VARARGS|METH_KEYWORDS,"do_alias_get_document"},
	{"get_line", (PyCFunction)DocumentOrderKey0_get_line, METH_NOARGS,"do_alias_get_line"},
	{"set_line", (PyCFunction)DocumentOrderKey0_set_line, METH_VARARGS|METH_KEYWORDS,"do_alias_get_line"},

 {NULL}  /* Sentinel */
};

static PyMethodDef DocumentOrderKey1_methods[] = {
	{"get_dtype", (PyCFunction)DocumentOrderKey1_get_dtype, METH_NOARGS,"do_alias_get_dtype"},
	{"set_dtype", (PyCFunction)DocumentOrderKey1_set_dtype, METH_VARARGS|METH_KEYWORDS,"do_alias_get_dtype"},
	{"get_sklad", (PyCFunction)DocumentOrderKey1_get_sklad, METH_NOARGS,"do_alias_get_sklad"},
	{"set_sklad", (PyCFunction)DocumentOrderKey1_set_sklad, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sklad"},
	{"get_document", (PyCFunction)DocumentOrderKey1_get_document, METH_NOARGS,"do_alias_get_document"},
	{"set_document", (PyCFunction)DocumentOrderKey1_set_document, METH_VARARGS|METH_KEYWORDS,"do_alias_get_document"},
	{"get_sort", (PyCFunction)DocumentOrderKey1_get_sort, METH_NOARGS,"do_alias_get_sort"},
	{"set_sort", (PyCFunction)DocumentOrderKey1_set_sort, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sort"},

 {NULL}  /* Sentinel */
};

static PyMethodDef DocumentOrderKey2_methods[] = {
	{"get_dtype", (PyCFunction)DocumentOrderKey2_get_dtype, METH_NOARGS,"do_alias_get_dtype"},
	{"set_dtype", (PyCFunction)DocumentOrderKey2_set_dtype, METH_VARARGS|METH_KEYWORDS,"do_alias_get_dtype"},
	{"get_sklad", (PyCFunction)DocumentOrderKey2_get_sklad, METH_NOARGS,"do_alias_get_sklad"},
	{"set_sklad", (PyCFunction)DocumentOrderKey2_set_sklad, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sklad"},
	{"get_document", (PyCFunction)DocumentOrderKey2_get_document, METH_NOARGS,"do_alias_get_document"},
	{"set_document", (PyCFunction)DocumentOrderKey2_set_document, METH_VARARGS|METH_KEYWORDS,"do_alias_get_document"},
	{"get_code", (PyCFunction)DocumentOrderKey2_get_code, METH_NOARGS,"do_alias_get_code"},
	{"set_code", (PyCFunction)DocumentOrderKey2_set_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_code"},

 {NULL}  /* Sentinel */
};

static PyMethodDef DocumentOrderKey3_methods[] = {
	{"get_code", (PyCFunction)DocumentOrderKey3_get_code, METH_NOARGS,"do_alias_get_code"},
	{"set_code", (PyCFunction)DocumentOrderKey3_set_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_code"},
	{"get_date", (PyCFunction)DocumentOrderKey3_get_date, METH_NOARGS,"do_alias_get_date"},
	{"set_date", (PyCFunction)DocumentOrderKey3_set_date, METH_VARARGS|METH_KEYWORDS,"do_alias_get_date"},
	{"get_time", (PyCFunction)DocumentOrderKey3_get_time, METH_NOARGS,"do_alias_get_time"},
	{"set_time", (PyCFunction)DocumentOrderKey3_set_time, METH_VARARGS|METH_KEYWORDS,"do_alias_get_time"},

 {NULL}  /* Sentinel */
};

static PyMethodDef DocumentOrderKey4_methods[] = {
	{"get_sklad", (PyCFunction)DocumentOrderKey4_get_sklad, METH_NOARGS,"do_alias_get_sklad"},
	{"set_sklad", (PyCFunction)DocumentOrderKey4_set_sklad, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sklad"},
	{"get_date", (PyCFunction)DocumentOrderKey4_get_date, METH_NOARGS,"do_alias_get_date"},
	{"set_date", (PyCFunction)DocumentOrderKey4_set_date, METH_VARARGS|METH_KEYWORDS,"do_alias_get_date"},
	{"get_time", (PyCFunction)DocumentOrderKey4_get_time, METH_NOARGS,"do_alias_get_time"},
	{"set_time", (PyCFunction)DocumentOrderKey4_set_time, METH_VARARGS|METH_KEYWORDS,"do_alias_get_time"},

 {NULL}  /* Sentinel */
};

static PyMethodDef DocumentLink_methods[] = {
	{"get_type", (PyCFunction)DocumentLink_get_type, METH_NOARGS,"do_alias_get_type"},
	{"set_type", (PyCFunction)DocumentLink_set_type, METH_VARARGS|METH_KEYWORDS,"do_alias_get_type"},
	{"get_m_dtype", (PyCFunction)DocumentLink_get_m_dtype, METH_NOARGS,"do_alias_get_m_dtype"},
	{"set_m_dtype", (PyCFunction)DocumentLink_set_m_dtype, METH_VARARGS|METH_KEYWORDS,"do_alias_get_m_dtype"},
	{"get_m_sklad", (PyCFunction)DocumentLink_get_m_sklad, METH_NOARGS,"do_alias_get_m_sklad"},
	{"set_m_sklad", (PyCFunction)DocumentLink_set_m_sklad, METH_VARARGS|METH_KEYWORDS,"do_alias_get_m_sklad"},
	{"get_m_document", (PyCFunction)DocumentLink_get_m_document, METH_NOARGS,"do_alias_get_m_document"},
	{"set_m_document", (PyCFunction)DocumentLink_set_m_document, METH_VARARGS|METH_KEYWORDS,"do_alias_get_m_document"},
	{"get_s_dtype", (PyCFunction)DocumentLink_get_s_dtype, METH_NOARGS,"do_alias_get_s_dtype"},
	{"set_s_dtype", (PyCFunction)DocumentLink_set_s_dtype, METH_VARARGS|METH_KEYWORDS,"do_alias_get_s_dtype"},
	{"get_s_sklad", (PyCFunction)DocumentLink_get_s_sklad, METH_NOARGS,"do_alias_get_s_sklad"},
	{"set_s_sklad", (PyCFunction)DocumentLink_set_s_sklad, METH_VARARGS|METH_KEYWORDS,"do_alias_get_s_sklad"},
	{"get_s_document", (PyCFunction)DocumentLink_get_s_document, METH_NOARGS,"do_alias_get_s_document"},
	{"set_s_document", (PyCFunction)DocumentLink_set_s_document, METH_VARARGS|METH_KEYWORDS,"do_alias_get_s_document"},
	{"get_sort", (PyCFunction)DocumentLink_get_sort, METH_NOARGS,"do_alias_get_sort"},
	{"set_sort", (PyCFunction)DocumentLink_set_sort, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sort"},

 {NULL}  /* Sentinel */
};

static PyMethodDef DocumentLinkKey0_methods[] = {
	{"get_m_dtype", (PyCFunction)DocumentLinkKey0_get_m_dtype, METH_NOARGS,"do_alias_get_m_dtype"},
	{"set_m_dtype", (PyCFunction)DocumentLinkKey0_set_m_dtype, METH_VARARGS|METH_KEYWORDS,"do_alias_get_m_dtype"},
	{"get_m_sklad", (PyCFunction)DocumentLinkKey0_get_m_sklad, METH_NOARGS,"do_alias_get_m_sklad"},
	{"set_m_sklad", (PyCFunction)DocumentLinkKey0_set_m_sklad, METH_VARARGS|METH_KEYWORDS,"do_alias_get_m_sklad"},
	{"get_m_document", (PyCFunction)DocumentLinkKey0_get_m_document, METH_NOARGS,"do_alias_get_m_document"},
	{"set_m_document", (PyCFunction)DocumentLinkKey0_set_m_document, METH_VARARGS|METH_KEYWORDS,"do_alias_get_m_document"},
	{"get_type", (PyCFunction)DocumentLinkKey0_get_type, METH_NOARGS,"do_alias_get_type"},
	{"set_type", (PyCFunction)DocumentLinkKey0_set_type, METH_VARARGS|METH_KEYWORDS,"do_alias_get_type"},
	{"get_sort", (PyCFunction)DocumentLinkKey0_get_sort, METH_NOARGS,"do_alias_get_sort"},
	{"set_sort", (PyCFunction)DocumentLinkKey0_set_sort, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sort"},

 {NULL}  /* Sentinel */
};

static PyMethodDef DocumentLinkKey1_methods[] = {
	{"get_s_dtype", (PyCFunction)DocumentLinkKey1_get_s_dtype, METH_NOARGS,"do_alias_get_s_dtype"},
	{"set_s_dtype", (PyCFunction)DocumentLinkKey1_set_s_dtype, METH_VARARGS|METH_KEYWORDS,"do_alias_get_s_dtype"},
	{"get_s_sklad", (PyCFunction)DocumentLinkKey1_get_s_sklad, METH_NOARGS,"do_alias_get_s_sklad"},
	{"set_s_sklad", (PyCFunction)DocumentLinkKey1_set_s_sklad, METH_VARARGS|METH_KEYWORDS,"do_alias_get_s_sklad"},
	{"get_s_document", (PyCFunction)DocumentLinkKey1_get_s_document, METH_NOARGS,"do_alias_get_s_document"},
	{"set_s_document", (PyCFunction)DocumentLinkKey1_set_s_document, METH_VARARGS|METH_KEYWORDS,"do_alias_get_s_document"},
	{"get_type", (PyCFunction)DocumentLinkKey1_get_type, METH_NOARGS,"do_alias_get_type"},
	{"set_type", (PyCFunction)DocumentLinkKey1_set_type, METH_VARARGS|METH_KEYWORDS,"do_alias_get_type"},
	{"get_sort", (PyCFunction)DocumentLinkKey1_get_sort, METH_NOARGS,"do_alias_get_sort"},
	{"set_sort", (PyCFunction)DocumentLinkKey1_set_sort, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sort"},

 {NULL}  /* Sentinel */
};

static PyMethodDef Enum_methods[] = {
	{"get_name", (PyCFunction)Enum_get_name, METH_NOARGS,"do_alias_get_name"},
	{"set_name", (PyCFunction)Enum_set_name, METH_VARARGS|METH_KEYWORDS,"do_alias_get_name"},
	{"get_lastvalue", (PyCFunction)Enum_get_lastvalue, METH_NOARGS,"do_alias_get_lastvalue"},
	{"set_lastvalue", (PyCFunction)Enum_set_lastvalue, METH_VARARGS|METH_KEYWORDS,"do_alias_get_lastvalue"},
	{"get_step", (PyCFunction)Enum_get_step, METH_NOARGS,"do_alias_get_step"},
	{"set_step", (PyCFunction)Enum_set_step, METH_VARARGS|METH_KEYWORDS,"do_alias_get_step"},
	{"get_LowBound", (PyCFunction)Enum_get_LowBound, METH_NOARGS,"do_alias_get_LowBound"},
	{"set_LowBound", (PyCFunction)Enum_set_LowBound, METH_VARARGS|METH_KEYWORDS,"do_alias_get_LowBound"},
	{"get_hiBound", (PyCFunction)Enum_get_hiBound, METH_NOARGS,"do_alias_get_hiBound"},
	{"set_hiBound", (PyCFunction)Enum_set_hiBound, METH_VARARGS|METH_KEYWORDS,"do_alias_get_hiBound"},
	{"get_cycleFl", (PyCFunction)Enum_get_cycleFl, METH_NOARGS,"do_alias_get_cycleFl"},
	{"set_cycleFl", (PyCFunction)Enum_set_cycleFl, METH_VARARGS|METH_KEYWORDS,"do_alias_get_cycleFl"},
	{"get_format", (PyCFunction)Enum_get_format, METH_NOARGS,"do_alias_get_format"},
	{"set_format", (PyCFunction)Enum_set_format, METH_VARARGS|METH_KEYWORDS,"do_alias_get_format"},

 {NULL}  /* Sentinel */
};

static PyMethodDef EnumKey0_methods[] = {
	{"get_name", (PyCFunction)EnumKey0_get_name, METH_NOARGS,"do_alias_get_name"},
	{"set_name", (PyCFunction)EnumKey0_set_name, METH_VARARGS|METH_KEYWORDS,"do_alias_get_name"},

 {NULL}  /* Sentinel */
};

static PyMethodDef DocumentView_methods[] = {
	{"get_type", (PyCFunction)DocumentView_get_type, METH_NOARGS,"do_alias_get_type"},
	{"set_type", (PyCFunction)DocumentView_set_type, METH_VARARGS|METH_KEYWORDS,"do_alias_get_type"},
	{"get_sort", (PyCFunction)DocumentView_get_sort, METH_NOARGS,"do_alias_get_sort"},
	{"set_sort", (PyCFunction)DocumentView_set_sort, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sort"},
	{"get_dtype", (PyCFunction)DocumentView_get_dtype, METH_NOARGS,"do_alias_get_dtype"},
	{"set_dtype", (PyCFunction)DocumentView_set_dtype, METH_VARARGS|METH_KEYWORDS,"do_alias_get_dtype"},
	{"get_sklad", (PyCFunction)DocumentView_get_sklad, METH_NOARGS,"do_alias_get_sklad"},
	{"set_sklad", (PyCFunction)DocumentView_set_sklad, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sklad"},
	{"get_document", (PyCFunction)DocumentView_get_document, METH_NOARGS,"do_alias_get_document"},
	{"set_document", (PyCFunction)DocumentView_set_document, METH_VARARGS|METH_KEYWORDS,"do_alias_get_document"},
	{"get_dtype1", (PyCFunction)DocumentView_get_dtype1, METH_NOARGS,"do_alias_get_dtype1"},
	{"set_dtype1", (PyCFunction)DocumentView_set_dtype1, METH_VARARGS|METH_KEYWORDS,"do_alias_get_dtype1"},

 {NULL}  /* Sentinel */
};

static PyMethodDef DocumentViewKey0_methods[] = {
	{"get_type", (PyCFunction)DocumentViewKey0_get_type, METH_NOARGS,"do_alias_get_type"},
	{"set_type", (PyCFunction)DocumentViewKey0_set_type, METH_VARARGS|METH_KEYWORDS,"do_alias_get_type"},
	{"get_sort", (PyCFunction)DocumentViewKey0_get_sort, METH_NOARGS,"do_alias_get_sort"},
	{"set_sort", (PyCFunction)DocumentViewKey0_set_sort, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sort"},
	{"get_dtype", (PyCFunction)DocumentViewKey0_get_dtype, METH_NOARGS,"do_alias_get_dtype"},
	{"set_dtype", (PyCFunction)DocumentViewKey0_set_dtype, METH_VARARGS|METH_KEYWORDS,"do_alias_get_dtype"},
	{"get_sklad", (PyCFunction)DocumentViewKey0_get_sklad, METH_NOARGS,"do_alias_get_sklad"},
	{"set_sklad", (PyCFunction)DocumentViewKey0_set_sklad, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sklad"},
	{"get_document", (PyCFunction)DocumentViewKey0_get_document, METH_NOARGS,"do_alias_get_document"},
	{"set_document", (PyCFunction)DocumentViewKey0_set_document, METH_VARARGS|METH_KEYWORDS,"do_alias_get_document"},

 {NULL}  /* Sentinel */
};

static PyMethodDef DocumentViewKey1_methods[] = {
	{"get_dtype", (PyCFunction)DocumentViewKey1_get_dtype, METH_NOARGS,"do_alias_get_dtype"},
	{"set_dtype", (PyCFunction)DocumentViewKey1_set_dtype, METH_VARARGS|METH_KEYWORDS,"do_alias_get_dtype"},
	{"get_sklad", (PyCFunction)DocumentViewKey1_get_sklad, METH_NOARGS,"do_alias_get_sklad"},
	{"set_sklad", (PyCFunction)DocumentViewKey1_set_sklad, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sklad"},
	{"get_document", (PyCFunction)DocumentViewKey1_get_document, METH_NOARGS,"do_alias_get_document"},
	{"set_document", (PyCFunction)DocumentViewKey1_set_document, METH_VARARGS|METH_KEYWORDS,"do_alias_get_document"},
	{"get_type", (PyCFunction)DocumentViewKey1_get_type, METH_NOARGS,"do_alias_get_type"},
	{"set_type", (PyCFunction)DocumentViewKey1_set_type, METH_VARARGS|METH_KEYWORDS,"do_alias_get_type"},

 {NULL}  /* Sentinel */
};

static PyMethodDef DocumentData_methods[] = {
	{"get_dtype", (PyCFunction)DocumentData_get_dtype, METH_NOARGS,"do_alias_get_dtype"},
	{"set_dtype", (PyCFunction)DocumentData_set_dtype, METH_VARARGS|METH_KEYWORDS,"do_alias_get_dtype"},
	{"get_sklad", (PyCFunction)DocumentData_get_sklad, METH_NOARGS,"do_alias_get_sklad"},
	{"set_sklad", (PyCFunction)DocumentData_set_sklad, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sklad"},
	{"get_document", (PyCFunction)DocumentData_get_document, METH_NOARGS,"do_alias_get_document"},
	{"set_document", (PyCFunction)DocumentData_set_document, METH_VARARGS|METH_KEYWORDS,"do_alias_get_document"},
	{"get_type", (PyCFunction)DocumentData_get_type, METH_NOARGS,"do_alias_get_type"},
	{"set_type", (PyCFunction)DocumentData_set_type, METH_VARARGS|METH_KEYWORDS,"do_alias_get_type"},
	{"get_number", (PyCFunction)DocumentData_get_number, METH_NOARGS,"do_alias_get_number"},
	{"set_number", (PyCFunction)DocumentData_set_number, METH_VARARGS|METH_KEYWORDS,"do_alias_get_number"},
	{"get_params", (PyCFunction)DocumentData_get_params, METH_NOARGS,"do_alias_get_params"},
	{"set_params", (PyCFunction)DocumentData_set_params, METH_VARARGS|METH_KEYWORDS,"do_alias_get_params"},

 {NULL}  /* Sentinel */
};

static PyMethodDef DocumentDataKey0_methods[] = {
	{"get_dtype", (PyCFunction)DocumentDataKey0_get_dtype, METH_NOARGS,"do_alias_get_dtype"},
	{"set_dtype", (PyCFunction)DocumentDataKey0_set_dtype, METH_VARARGS|METH_KEYWORDS,"do_alias_get_dtype"},
	{"get_sklad", (PyCFunction)DocumentDataKey0_get_sklad, METH_NOARGS,"do_alias_get_sklad"},
	{"set_sklad", (PyCFunction)DocumentDataKey0_set_sklad, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sklad"},
	{"get_document", (PyCFunction)DocumentDataKey0_get_document, METH_NOARGS,"do_alias_get_document"},
	{"set_document", (PyCFunction)DocumentDataKey0_set_document, METH_VARARGS|METH_KEYWORDS,"do_alias_get_document"},
	{"get_type", (PyCFunction)DocumentDataKey0_get_type, METH_NOARGS,"do_alias_get_type"},
	{"set_type", (PyCFunction)DocumentDataKey0_set_type, METH_VARARGS|METH_KEYWORDS,"do_alias_get_type"},
	{"get_number", (PyCFunction)DocumentDataKey0_get_number, METH_NOARGS,"do_alias_get_number"},
	{"set_number", (PyCFunction)DocumentDataKey0_set_number, METH_VARARGS|METH_KEYWORDS,"do_alias_get_number"},

 {NULL}  /* Sentinel */
};

static PyMethodDef ProductLink_methods[] = {
	{"get_type", (PyCFunction)ProductLink_get_type, METH_NOARGS,"do_alias_get_type"},
	{"set_type", (PyCFunction)ProductLink_set_type, METH_VARARGS|METH_KEYWORDS,"do_alias_get_type"},
	{"get_m_code", (PyCFunction)ProductLink_get_m_code, METH_NOARGS,"do_alias_get_m_code"},
	{"set_m_code", (PyCFunction)ProductLink_set_m_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_m_code"},
	{"get_s_code", (PyCFunction)ProductLink_get_s_code, METH_NOARGS,"do_alias_get_s_code"},
	{"set_s_code", (PyCFunction)ProductLink_set_s_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_s_code"},
	{"get_number", (PyCFunction)ProductLink_get_number, METH_NOARGS,"do_alias_get_number"},
	{"set_number", (PyCFunction)ProductLink_set_number, METH_VARARGS|METH_KEYWORDS,"do_alias_get_number"},
	{"get_qty", (PyCFunction)ProductLink_get_qty, METH_NOARGS,"do_alias_get_qty"},
	{"set_qty", (PyCFunction)ProductLink_set_qty, METH_VARARGS|METH_KEYWORDS,"do_alias_get_qty"},
	{"get_params", (PyCFunction)ProductLink_get_params, METH_NOARGS,"do_alias_get_params"},
	{"set_params", (PyCFunction)ProductLink_set_params, METH_VARARGS|METH_KEYWORDS,"do_alias_get_params"},

 {NULL}  /* Sentinel */
};

static PyMethodDef ProductLinkKey0_methods[] = {
	{"get_m_code", (PyCFunction)ProductLinkKey0_get_m_code, METH_NOARGS,"do_alias_get_m_code"},
	{"set_m_code", (PyCFunction)ProductLinkKey0_set_m_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_m_code"},
	{"get_type", (PyCFunction)ProductLinkKey0_get_type, METH_NOARGS,"do_alias_get_type"},
	{"set_type", (PyCFunction)ProductLinkKey0_set_type, METH_VARARGS|METH_KEYWORDS,"do_alias_get_type"},
	{"get_number", (PyCFunction)ProductLinkKey0_get_number, METH_NOARGS,"do_alias_get_number"},
	{"set_number", (PyCFunction)ProductLinkKey0_set_number, METH_VARARGS|METH_KEYWORDS,"do_alias_get_number"},

 {NULL}  /* Sentinel */
};

static PyMethodDef ProductLinkKey1_methods[] = {
	{"get_s_code", (PyCFunction)ProductLinkKey1_get_s_code, METH_NOARGS,"do_alias_get_s_code"},
	{"set_s_code", (PyCFunction)ProductLinkKey1_set_s_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_s_code"},
	{"get_type", (PyCFunction)ProductLinkKey1_get_type, METH_NOARGS,"do_alias_get_type"},
	{"set_type", (PyCFunction)ProductLinkKey1_set_type, METH_VARARGS|METH_KEYWORDS,"do_alias_get_type"},

 {NULL}  /* Sentinel */
};

static PyMethodDef ProductView_methods[] = {
	{"get_type", (PyCFunction)ProductView_get_type, METH_NOARGS,"do_alias_get_type"},
	{"set_type", (PyCFunction)ProductView_set_type, METH_VARARGS|METH_KEYWORDS,"do_alias_get_type"},
	{"get_code", (PyCFunction)ProductView_get_code, METH_NOARGS,"do_alias_get_code"},
	{"set_code", (PyCFunction)ProductView_set_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_code"},
	{"get_barcode", (PyCFunction)ProductView_get_barcode, METH_NOARGS,"do_alias_get_barcode"},
	{"set_barcode", (PyCFunction)ProductView_set_barcode, METH_VARARGS|METH_KEYWORDS,"do_alias_get_barcode"},
	{"get_sklad", (PyCFunction)ProductView_get_sklad, METH_NOARGS,"do_alias_get_sklad"},
	{"set_sklad", (PyCFunction)ProductView_set_sklad, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sklad"},
	{"get_sort", (PyCFunction)ProductView_get_sort, METH_NOARGS,"do_alias_get_sort"},
	{"set_sort", (PyCFunction)ProductView_set_sort, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sort"},

 {NULL}  /* Sentinel */
};

static PyMethodDef ProductViewKey0_methods[] = {
	{"get_type", (PyCFunction)ProductViewKey0_get_type, METH_NOARGS,"do_alias_get_type"},
	{"set_type", (PyCFunction)ProductViewKey0_set_type, METH_VARARGS|METH_KEYWORDS,"do_alias_get_type"},
	{"get_sklad", (PyCFunction)ProductViewKey0_get_sklad, METH_NOARGS,"do_alias_get_sklad"},
	{"set_sklad", (PyCFunction)ProductViewKey0_set_sklad, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sklad"},
	{"get_sort", (PyCFunction)ProductViewKey0_get_sort, METH_NOARGS,"do_alias_get_sort"},
	{"set_sort", (PyCFunction)ProductViewKey0_set_sort, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sort"},
	{"get_code", (PyCFunction)ProductViewKey0_get_code, METH_NOARGS,"do_alias_get_code"},
	{"set_code", (PyCFunction)ProductViewKey0_set_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_code"},

 {NULL}  /* Sentinel */
};

static PyMethodDef ProductViewKey1_methods[] = {
	{"get_code", (PyCFunction)ProductViewKey1_get_code, METH_NOARGS,"do_alias_get_code"},
	{"set_code", (PyCFunction)ProductViewKey1_set_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_code"},
	{"get_sklad", (PyCFunction)ProductViewKey1_get_sklad, METH_NOARGS,"do_alias_get_sklad"},
	{"set_sklad", (PyCFunction)ProductViewKey1_set_sklad, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sklad"},

 {NULL}  /* Sentinel */
};

static PyMethodDef ProductData_methods[] = {
	{"get_code", (PyCFunction)ProductData_get_code, METH_NOARGS,"do_alias_get_code"},
	{"set_code", (PyCFunction)ProductData_set_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_code"},
	{"get_type", (PyCFunction)ProductData_get_type, METH_NOARGS,"do_alias_get_type"},
	{"set_type", (PyCFunction)ProductData_set_type, METH_VARARGS|METH_KEYWORDS,"do_alias_get_type"},
	{"get_number", (PyCFunction)ProductData_get_number, METH_NOARGS,"do_alias_get_number"},
	{"set_number", (PyCFunction)ProductData_set_number, METH_VARARGS|METH_KEYWORDS,"do_alias_get_number"},
	{"get_params", (PyCFunction)ProductData_get_params, METH_NOARGS,"do_alias_get_params"},
	{"set_params", (PyCFunction)ProductData_set_params, METH_VARARGS|METH_KEYWORDS,"do_alias_get_params"},

 {NULL}  /* Sentinel */
};

static PyMethodDef ProductDataKey0_methods[] = {
	{"get_code", (PyCFunction)ProductDataKey0_get_code, METH_NOARGS,"do_alias_get_code"},
	{"set_code", (PyCFunction)ProductDataKey0_set_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_code"},
	{"get_type", (PyCFunction)ProductDataKey0_get_type, METH_NOARGS,"do_alias_get_type"},
	{"set_type", (PyCFunction)ProductDataKey0_set_type, METH_VARARGS|METH_KEYWORDS,"do_alias_get_type"},
	{"get_number", (PyCFunction)ProductDataKey0_get_number, METH_NOARGS,"do_alias_get_number"},
	{"set_number", (PyCFunction)ProductDataKey0_set_number, METH_VARARGS|METH_KEYWORDS,"do_alias_get_number"},

 {NULL}  /* Sentinel */
};

static PyMethodDef Barcode_methods[] = {
	{"get_barcode", (PyCFunction)Barcode_get_barcode, METH_NOARGS,"do_alias_get_barcode"},
	{"set_barcode", (PyCFunction)Barcode_set_barcode, METH_VARARGS|METH_KEYWORDS,"do_alias_get_barcode"},
	{"get_code", (PyCFunction)Barcode_get_code, METH_NOARGS,"do_alias_get_code"},
	{"set_code", (PyCFunction)Barcode_set_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_code"},
	{"get_total", (PyCFunction)Barcode_get_total, METH_NOARGS,"do_alias_get_total"},
	{"set_total", (PyCFunction)Barcode_set_total, METH_VARARGS|METH_KEYWORDS,"do_alias_get_total"},
	{"get_sk", (PyCFunction)Barcode_get_sk, METH_NOARGS,"do_alias_get_sk"},
	{"set_sk", (PyCFunction)Barcode_set_sk, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sk"},

 {NULL}  /* Sentinel */
};

static PyMethodDef BarcodeKey0_methods[] = {
	{"get_barcode", (PyCFunction)BarcodeKey0_get_barcode, METH_NOARGS,"do_alias_get_barcode"},
	{"set_barcode", (PyCFunction)BarcodeKey0_set_barcode, METH_VARARGS|METH_KEYWORDS,"do_alias_get_barcode"},

 {NULL}  /* Sentinel */
};

static PyMethodDef BarcodeKey1_methods[] = {
	{"get_code", (PyCFunction)BarcodeKey1_get_code, METH_NOARGS,"do_alias_get_code"},
	{"set_code", (PyCFunction)BarcodeKey1_set_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_code"},

 {NULL}  /* Sentinel */
};

static PyMethodDef Account_methods[] = {
	{"get_account", (PyCFunction)Account_get_account, METH_NOARGS,"do_alias_get_account"},
	{"set_account", (PyCFunction)Account_set_account, METH_VARARGS|METH_KEYWORDS,"do_alias_get_account"},
	{"get_ticacc", (PyCFunction)Account_get_ticacc, METH_NOARGS,"do_alias_get_ticacc"},
	{"set_ticacc", (PyCFunction)Account_set_ticacc, METH_VARARGS|METH_KEYWORDS,"do_alias_get_ticacc"},
	{"get_name", (PyCFunction)Account_get_name, METH_NOARGS,"do_alias_get_name"},
	{"set_name", (PyCFunction)Account_set_name, METH_VARARGS|METH_KEYWORDS,"do_alias_get_name"},
	{"get_debit", (PyCFunction)Account_get_debit, METH_NOARGS,"do_alias_get_debit"},
	{"set_debit", (PyCFunction)Account_set_debit, METH_VARARGS|METH_KEYWORDS,"do_alias_get_debit"},
	{"get_credit", (PyCFunction)Account_get_credit, METH_NOARGS,"do_alias_get_credit"},
	{"set_credit", (PyCFunction)Account_set_credit, METH_VARARGS|METH_KEYWORDS,"do_alias_get_credit"},
	{"get_positivesaldo", (PyCFunction)Account_get_positivesaldo, METH_NOARGS,"do_alias_get_positivesaldo"},
	{"set_positivesaldo", (PyCFunction)Account_set_positivesaldo, METH_VARARGS|METH_KEYWORDS,"do_alias_get_positivesaldo"},
	{"get_itog", (PyCFunction)Account_get_itog, METH_NOARGS,"do_alias_get_itog"},
	{"set_itog", (PyCFunction)Account_set_itog, METH_VARARGS|METH_KEYWORDS,"do_alias_get_itog"},
	{"get_analitsub", (PyCFunction)Account_get_analitsub, METH_NOARGS,"do_alias_get_analitsub"},
	{"set_analitsub", (PyCFunction)Account_set_analitsub, METH_VARARGS|METH_KEYWORDS,"do_alias_get_analitsub"},
	{"get_analitpartner", (PyCFunction)Account_get_analitpartner, METH_NOARGS,"do_alias_get_analitpartner"},
	{"set_analitpartner", (PyCFunction)Account_set_analitpartner, METH_VARARGS|METH_KEYWORDS,"do_alias_get_analitpartner"},
	{"get_analitother", (PyCFunction)Account_get_analitother, METH_NOARGS,"do_alias_get_analitother"},
	{"set_analitother", (PyCFunction)Account_set_analitother, METH_VARARGS|METH_KEYWORDS,"do_alias_get_analitother"},
	{"get_othername", (PyCFunction)Account_get_othername, METH_NOARGS,"do_alias_get_othername"},
	{"set_othername", (PyCFunction)Account_set_othername, METH_VARARGS|METH_KEYWORDS,"do_alias_get_othername"},
	{"get_sumincl", (PyCFunction)Account_get_sumincl, METH_NOARGS,"do_alias_get_sumincl"},
	{"set_sumincl", (PyCFunction)Account_set_sumincl, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sumincl"},

 {NULL}  /* Sentinel */
};

static PyMethodDef AccountKey0_methods[] = {
	{"get_account", (PyCFunction)AccountKey0_get_account, METH_NOARGS,"do_alias_get_account"},
	{"set_account", (PyCFunction)AccountKey0_set_account, METH_VARARGS|METH_KEYWORDS,"do_alias_get_account"},
	{"get_itog", (PyCFunction)AccountKey0_get_itog, METH_NOARGS,"do_alias_get_itog"},
	{"set_itog", (PyCFunction)AccountKey0_set_itog, METH_VARARGS|METH_KEYWORDS,"do_alias_get_itog"},

 {NULL}  /* Sentinel */
};

static PyMethodDef Subaccount_methods[] = {
	{"get_account", (PyCFunction)Subaccount_get_account, METH_NOARGS,"do_alias_get_account"},
	{"set_account", (PyCFunction)Subaccount_set_account, METH_VARARGS|METH_KEYWORDS,"do_alias_get_account"},
	{"get_subaccount", (PyCFunction)Subaccount_get_subaccount, METH_NOARGS,"do_alias_get_subaccount"},
	{"set_subaccount", (PyCFunction)Subaccount_set_subaccount, METH_VARARGS|METH_KEYWORDS,"do_alias_get_subaccount"},
	{"get_ticacc", (PyCFunction)Subaccount_get_ticacc, METH_NOARGS,"do_alias_get_ticacc"},
	{"set_ticacc", (PyCFunction)Subaccount_set_ticacc, METH_VARARGS|METH_KEYWORDS,"do_alias_get_ticacc"},
	{"get_name", (PyCFunction)Subaccount_get_name, METH_NOARGS,"do_alias_get_name"},
	{"set_name", (PyCFunction)Subaccount_set_name, METH_VARARGS|METH_KEYWORDS,"do_alias_get_name"},
	{"get_debit", (PyCFunction)Subaccount_get_debit, METH_NOARGS,"do_alias_get_debit"},
	{"set_debit", (PyCFunction)Subaccount_set_debit, METH_VARARGS|METH_KEYWORDS,"do_alias_get_debit"},
	{"get_credit", (PyCFunction)Subaccount_get_credit, METH_NOARGS,"do_alias_get_credit"},
	{"set_credit", (PyCFunction)Subaccount_set_credit, METH_VARARGS|METH_KEYWORDS,"do_alias_get_credit"},
	{"get_positivesaldo", (PyCFunction)Subaccount_get_positivesaldo, METH_NOARGS,"do_alias_get_positivesaldo"},
	{"set_positivesaldo", (PyCFunction)Subaccount_set_positivesaldo, METH_VARARGS|METH_KEYWORDS,"do_alias_get_positivesaldo"},
	{"get_itog", (PyCFunction)Subaccount_get_itog, METH_NOARGS,"do_alias_get_itog"},
	{"set_itog", (PyCFunction)Subaccount_set_itog, METH_VARARGS|METH_KEYWORDS,"do_alias_get_itog"},
	{"get_sumincl", (PyCFunction)Subaccount_get_sumincl, METH_NOARGS,"do_alias_get_sumincl"},
	{"set_sumincl", (PyCFunction)Subaccount_set_sumincl, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sumincl"},

 {NULL}  /* Sentinel */
};

static PyMethodDef SubaccountKey0_methods[] = {
	{"get_account", (PyCFunction)SubaccountKey0_get_account, METH_NOARGS,"do_alias_get_account"},
	{"set_account", (PyCFunction)SubaccountKey0_set_account, METH_VARARGS|METH_KEYWORDS,"do_alias_get_account"},
	{"get_subaccount", (PyCFunction)SubaccountKey0_get_subaccount, METH_NOARGS,"do_alias_get_subaccount"},
	{"set_subaccount", (PyCFunction)SubaccountKey0_set_subaccount, METH_VARARGS|METH_KEYWORDS,"do_alias_get_subaccount"},
	{"get_itog", (PyCFunction)SubaccountKey0_get_itog, METH_NOARGS,"do_alias_get_itog"},
	{"set_itog", (PyCFunction)SubaccountKey0_set_itog, METH_VARARGS|METH_KEYWORDS,"do_alias_get_itog"},

 {NULL}  /* Sentinel */
};

static PyMethodDef Group_methods[] = {
	{"get_group_id", (PyCFunction)Group_get_group_id, METH_NOARGS,"do_alias_get_group_id"},
	{"set_group_id", (PyCFunction)Group_set_group_id, METH_VARARGS|METH_KEYWORDS,"do_alias_get_group_id"},
	{"get_name", (PyCFunction)Group_get_name, METH_NOARGS,"do_alias_get_name"},
	{"set_name", (PyCFunction)Group_set_name, METH_VARARGS|METH_KEYWORDS,"do_alias_get_name"},
	{"get_class_id", (PyCFunction)Group_get_class_id, METH_NOARGS,"do_alias_get_class_id"},
	{"set_class_id", (PyCFunction)Group_set_class_id, METH_VARARGS|METH_KEYWORDS,"do_alias_get_class_id"},

 {NULL}  /* Sentinel */
};

static PyMethodDef GroupKey0_methods[] = {
	{"get_class_id", (PyCFunction)GroupKey0_get_class_id, METH_NOARGS,"do_alias_get_class_id"},
	{"set_class_id", (PyCFunction)GroupKey0_set_class_id, METH_VARARGS|METH_KEYWORDS,"do_alias_get_class_id"},
	{"get_group_id", (PyCFunction)GroupKey0_get_group_id, METH_NOARGS,"do_alias_get_group_id"},
	{"set_group_id", (PyCFunction)GroupKey0_set_group_id, METH_VARARGS|METH_KEYWORDS,"do_alias_get_group_id"},

 {NULL}  /* Sentinel */
};

static PyMethodDef GroupKey1_methods[] = {
	{"get_class_id", (PyCFunction)GroupKey1_get_class_id, METH_NOARGS,"do_alias_get_class_id"},
	{"set_class_id", (PyCFunction)GroupKey1_set_class_id, METH_VARARGS|METH_KEYWORDS,"do_alias_get_class_id"},
	{"get_name", (PyCFunction)GroupKey1_get_name, METH_NOARGS,"do_alias_get_name"},
	{"set_name", (PyCFunction)GroupKey1_set_name, METH_VARARGS|METH_KEYWORDS,"do_alias_get_name"},

 {NULL}  /* Sentinel */
};

static PyMethodDef Subgroup_methods[] = {
	{"get_group_id", (PyCFunction)Subgroup_get_group_id, METH_NOARGS,"do_alias_get_group_id"},
	{"set_group_id", (PyCFunction)Subgroup_set_group_id, METH_VARARGS|METH_KEYWORDS,"do_alias_get_group_id"},
	{"get_subgroup_id", (PyCFunction)Subgroup_get_subgroup_id, METH_NOARGS,"do_alias_get_subgroup_id"},
	{"set_subgroup_id", (PyCFunction)Subgroup_set_subgroup_id, METH_VARARGS|METH_KEYWORDS,"do_alias_get_subgroup_id"},
	{"get_name", (PyCFunction)Subgroup_get_name, METH_NOARGS,"do_alias_get_name"},
	{"set_name", (PyCFunction)Subgroup_set_name, METH_VARARGS|METH_KEYWORDS,"do_alias_get_name"},
	{"get_class_id", (PyCFunction)Subgroup_get_class_id, METH_NOARGS,"do_alias_get_class_id"},
	{"set_class_id", (PyCFunction)Subgroup_set_class_id, METH_VARARGS|METH_KEYWORDS,"do_alias_get_class_id"},
	{"get_stat", (PyCFunction)Subgroup_get_stat, METH_NOARGS,"do_alias_get_stat"},
	{"set_stat", (PyCFunction)Subgroup_set_stat, METH_VARARGS|METH_KEYWORDS,"do_alias_get_stat"},
	{"get_params", (PyCFunction)Subgroup_get_params, METH_NOARGS,"do_alias_get_params"},
	{"set_params", (PyCFunction)Subgroup_set_params, METH_VARARGS|METH_KEYWORDS,"do_alias_get_params"},

 {NULL}  /* Sentinel */
};

static PyMethodDef SubgroupKey0_methods[] = {
	{"get_class_id", (PyCFunction)SubgroupKey0_get_class_id, METH_NOARGS,"do_alias_get_class_id"},
	{"set_class_id", (PyCFunction)SubgroupKey0_set_class_id, METH_VARARGS|METH_KEYWORDS,"do_alias_get_class_id"},
	{"get_group_id", (PyCFunction)SubgroupKey0_get_group_id, METH_NOARGS,"do_alias_get_group_id"},
	{"set_group_id", (PyCFunction)SubgroupKey0_set_group_id, METH_VARARGS|METH_KEYWORDS,"do_alias_get_group_id"},
	{"get_subgroup_id", (PyCFunction)SubgroupKey0_get_subgroup_id, METH_NOARGS,"do_alias_get_subgroup_id"},
	{"set_subgroup_id", (PyCFunction)SubgroupKey0_set_subgroup_id, METH_VARARGS|METH_KEYWORDS,"do_alias_get_subgroup_id"},

 {NULL}  /* Sentinel */
};

static PyMethodDef SubgroupKey1_methods[] = {
	{"get_class_id", (PyCFunction)SubgroupKey1_get_class_id, METH_NOARGS,"do_alias_get_class_id"},
	{"set_class_id", (PyCFunction)SubgroupKey1_set_class_id, METH_VARARGS|METH_KEYWORDS,"do_alias_get_class_id"},
	{"get_group_id", (PyCFunction)SubgroupKey1_get_group_id, METH_NOARGS,"do_alias_get_group_id"},
	{"set_group_id", (PyCFunction)SubgroupKey1_set_group_id, METH_VARARGS|METH_KEYWORDS,"do_alias_get_group_id"},
	{"get_name", (PyCFunction)SubgroupKey1_get_name, METH_NOARGS,"do_alias_get_name"},
	{"set_name", (PyCFunction)SubgroupKey1_set_name, METH_VARARGS|METH_KEYWORDS,"do_alias_get_name"},

 {NULL}  /* Sentinel */
};

static PyMethodDef Region_methods[] = {
	{"get_code", (PyCFunction)Region_get_code, METH_NOARGS,"do_alias_get_code"},
	{"set_code", (PyCFunction)Region_set_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_code"},
	{"get_name", (PyCFunction)Region_get_name, METH_NOARGS,"do_alias_get_name"},
	{"set_name", (PyCFunction)Region_set_name, METH_VARARGS|METH_KEYWORDS,"do_alias_get_name"},
	{"get_stat", (PyCFunction)Region_get_stat, METH_NOARGS,"do_alias_get_stat"},
	{"set_stat", (PyCFunction)Region_set_stat, METH_VARARGS|METH_KEYWORDS,"do_alias_get_stat"},
	{"get_params", (PyCFunction)Region_get_params, METH_NOARGS,"do_alias_get_params"},
	{"set_params", (PyCFunction)Region_set_params, METH_VARARGS|METH_KEYWORDS,"do_alias_get_params"},

 {NULL}  /* Sentinel */
};

static PyMethodDef RegionKey0_methods[] = {
	{"get_code", (PyCFunction)RegionKey0_get_code, METH_NOARGS,"do_alias_get_code"},
	{"set_code", (PyCFunction)RegionKey0_set_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_code"},

 {NULL}  /* Sentinel */
};

static PyMethodDef RegionKey1_methods[] = {
	{"get_name", (PyCFunction)RegionKey1_get_name, METH_NOARGS,"do_alias_get_name"},
	{"set_name", (PyCFunction)RegionKey1_set_name, METH_VARARGS|METH_KEYWORDS,"do_alias_get_name"},

 {NULL}  /* Sentinel */
};

static PyMethodDef Otdel_methods[] = {
	{"get_code", (PyCFunction)Otdel_get_code, METH_NOARGS,"do_alias_get_code"},
	{"set_code", (PyCFunction)Otdel_set_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_code"},
	{"get_name", (PyCFunction)Otdel_get_name, METH_NOARGS,"do_alias_get_name"},
	{"set_name", (PyCFunction)Otdel_set_name, METH_VARARGS|METH_KEYWORDS,"do_alias_get_name"},
	{"get_params", (PyCFunction)Otdel_get_params, METH_NOARGS,"do_alias_get_params"},
	{"set_params", (PyCFunction)Otdel_set_params, METH_VARARGS|METH_KEYWORDS,"do_alias_get_params"},

 {NULL}  /* Sentinel */
};

static PyMethodDef OtdelKey0_methods[] = {
	{"get_code", (PyCFunction)OtdelKey0_get_code, METH_NOARGS,"do_alias_get_code"},
	{"set_code", (PyCFunction)OtdelKey0_set_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_code"},

 {NULL}  /* Sentinel */
};

static PyMethodDef Partner_methods[] = {
	{"get_g_code", (PyCFunction)Partner_get_g_code, METH_NOARGS,"do_alias_get_g_code"},
	{"set_g_code", (PyCFunction)Partner_set_g_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_g_code"},
	{"get_code", (PyCFunction)Partner_get_code, METH_NOARGS,"do_alias_get_code"},
	{"set_code", (PyCFunction)Partner_set_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_code"},
	{"get_otdel", (PyCFunction)Partner_get_otdel, METH_NOARGS,"do_alias_get_otdel"},
	{"set_otdel", (PyCFunction)Partner_set_otdel, METH_VARARGS|METH_KEYWORDS,"do_alias_get_otdel"},
	{"get_sklad", (PyCFunction)Partner_get_sklad, METH_NOARGS,"do_alias_get_sklad"},
	{"set_sklad", (PyCFunction)Partner_set_sklad, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sklad"},
	{"get_name", (PyCFunction)Partner_get_name, METH_NOARGS,"do_alias_get_name"},
	{"set_name", (PyCFunction)Partner_set_name, METH_VARARGS|METH_KEYWORDS,"do_alias_get_name"},
	{"get_bank", (PyCFunction)Partner_get_bank, METH_NOARGS,"do_alias_get_bank"},
	{"set_bank", (PyCFunction)Partner_set_bank, METH_VARARGS|METH_KEYWORDS,"do_alias_get_bank"},
	{"get_MFO", (PyCFunction)Partner_get_MFO, METH_NOARGS,"do_alias_get_MFO"},
	{"set_MFO", (PyCFunction)Partner_set_MFO, METH_VARARGS|METH_KEYWORDS,"do_alias_get_MFO"},
	{"get_SHET", (PyCFunction)Partner_get_SHET, METH_NOARGS,"do_alias_get_SHET"},
	{"set_SHET", (PyCFunction)Partner_set_SHET, METH_VARARGS|METH_KEYWORDS,"do_alias_get_SHET"},
	{"get_ADRES", (PyCFunction)Partner_get_ADRES, METH_NOARGS,"do_alias_get_ADRES"},
	{"set_ADRES", (PyCFunction)Partner_set_ADRES, METH_VARARGS|METH_KEYWORDS,"do_alias_get_ADRES"},
	{"get_PLAT_PGR", (PyCFunction)Partner_get_PLAT_PGR, METH_NOARGS,"do_alias_get_PLAT_PGR"},
	{"set_PLAT_PGR", (PyCFunction)Partner_set_PLAT_PGR, METH_VARARGS|METH_KEYWORDS,"do_alias_get_PLAT_PGR"},
	{"get_PLAT_PAR", (PyCFunction)Partner_get_PLAT_PAR, METH_NOARGS,"do_alias_get_PLAT_PAR"},
	{"set_PLAT_PAR", (PyCFunction)Partner_set_PLAT_PAR, METH_VARARGS|METH_KEYWORDS,"do_alias_get_PLAT_PAR"},
	{"get_KSHET", (PyCFunction)Partner_get_KSHET, METH_NOARGS,"do_alias_get_KSHET"},
	{"set_KSHET", (PyCFunction)Partner_set_KSHET, METH_VARARGS|METH_KEYWORDS,"do_alias_get_KSHET"},
	{"get_fio", (PyCFunction)Partner_get_fio, METH_NOARGS,"do_alias_get_fio"},
	{"set_fio", (PyCFunction)Partner_set_fio, METH_VARARGS|METH_KEYWORDS,"do_alias_get_fio"},
	{"get_SUM", (PyCFunction)Partner_get_SUM, METH_NOARGS,"do_alias_get_SUM"},
	{"set_SUM", (PyCFunction)Partner_set_SUM, METH_VARARGS|METH_KEYWORDS,"do_alias_get_SUM"},
	{"get_codestr", (PyCFunction)Partner_get_codestr, METH_NOARGS,"do_alias_get_codestr"},
	{"set_codestr", (PyCFunction)Partner_set_codestr, METH_VARARGS|METH_KEYWORDS,"do_alias_get_codestr"},
	{"get_state", (PyCFunction)Partner_get_state, METH_NOARGS,"do_alias_get_state"},
	{"set_state", (PyCFunction)Partner_set_state, METH_VARARGS|METH_KEYWORDS,"do_alias_get_state"},
	{"get_type", (PyCFunction)Partner_get_type, METH_NOARGS,"do_alias_get_type"},
	{"set_type", (PyCFunction)Partner_set_type, METH_VARARGS|METH_KEYWORDS,"do_alias_get_type"},
	{"get_Klass", (PyCFunction)Partner_get_Klass, METH_NOARGS,"do_alias_get_Klass"},
	{"set_Klass", (PyCFunction)Partner_set_Klass, METH_VARARGS|METH_KEYWORDS,"do_alias_get_Klass"},
	{"get_Param", (PyCFunction)Partner_get_Param, METH_NOARGS,"do_alias_get_Param"},
	{"set_Param", (PyCFunction)Partner_set_Param, METH_VARARGS|METH_KEYWORDS,"do_alias_get_Param"},
	{"get_Sum_N", (PyCFunction)Partner_get_Sum_N, METH_NOARGS,"do_alias_get_Sum_N"},
	{"set_Sum_N", (PyCFunction)Partner_set_Sum_N, METH_VARARGS|METH_KEYWORDS,"do_alias_get_Sum_N"},
	{"get_params", (PyCFunction)Partner_get_params, METH_NOARGS,"do_alias_get_params"},
	{"set_params", (PyCFunction)Partner_set_params, METH_VARARGS|METH_KEYWORDS,"do_alias_get_params"},

 {NULL}  /* Sentinel */
};

static PyMethodDef PartnerKey0_methods[] = {
	{"get_g_code", (PyCFunction)PartnerKey0_get_g_code, METH_NOARGS,"do_alias_get_g_code"},
	{"set_g_code", (PyCFunction)PartnerKey0_set_g_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_g_code"},
	{"get_code", (PyCFunction)PartnerKey0_get_code, METH_NOARGS,"do_alias_get_code"},
	{"set_code", (PyCFunction)PartnerKey0_set_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_code"},

 {NULL}  /* Sentinel */
};

static PyMethodDef PartnerKey1_methods[] = {
	{"get_g_code", (PyCFunction)PartnerKey1_get_g_code, METH_NOARGS,"do_alias_get_g_code"},
	{"set_g_code", (PyCFunction)PartnerKey1_set_g_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_g_code"},
	{"get_name", (PyCFunction)PartnerKey1_get_name, METH_NOARGS,"do_alias_get_name"},
	{"set_name", (PyCFunction)PartnerKey1_set_name, METH_VARARGS|METH_KEYWORDS,"do_alias_get_name"},

 {NULL}  /* Sentinel */
};

static PyMethodDef PartnerKey2_methods[] = {
	{"get_name", (PyCFunction)PartnerKey2_get_name, METH_NOARGS,"do_alias_get_name"},
	{"set_name", (PyCFunction)PartnerKey2_set_name, METH_VARARGS|METH_KEYWORDS,"do_alias_get_name"},

 {NULL}  /* Sentinel */
};

static PyMethodDef PartnerKey3_methods[] = {
	{"get_code", (PyCFunction)PartnerKey3_get_code, METH_NOARGS,"do_alias_get_code"},
	{"set_code", (PyCFunction)PartnerKey3_set_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_code"},

 {NULL}  /* Sentinel */
};

static PyMethodDef PartnerData_methods[] = {
	{"get_g_code", (PyCFunction)PartnerData_get_g_code, METH_NOARGS,"do_alias_get_g_code"},
	{"set_g_code", (PyCFunction)PartnerData_set_g_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_g_code"},
	{"get_code", (PyCFunction)PartnerData_get_code, METH_NOARGS,"do_alias_get_code"},
	{"set_code", (PyCFunction)PartnerData_set_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_code"},
	{"get_type", (PyCFunction)PartnerData_get_type, METH_NOARGS,"do_alias_get_type"},
	{"set_type", (PyCFunction)PartnerData_set_type, METH_VARARGS|METH_KEYWORDS,"do_alias_get_type"},
	{"get_number", (PyCFunction)PartnerData_get_number, METH_NOARGS,"do_alias_get_number"},
	{"set_number", (PyCFunction)PartnerData_set_number, METH_VARARGS|METH_KEYWORDS,"do_alias_get_number"},
	{"get_date", (PyCFunction)PartnerData_get_date, METH_NOARGS,"do_alias_get_date"},
	{"set_date", (PyCFunction)PartnerData_set_date, METH_VARARGS|METH_KEYWORDS,"do_alias_get_date"},
	{"get_time", (PyCFunction)PartnerData_get_time, METH_NOARGS,"do_alias_get_time"},
	{"set_time", (PyCFunction)PartnerData_set_time, METH_VARARGS|METH_KEYWORDS,"do_alias_get_time"},
	{"get_params", (PyCFunction)PartnerData_get_params, METH_NOARGS,"do_alias_get_params"},
	{"set_params", (PyCFunction)PartnerData_set_params, METH_VARARGS|METH_KEYWORDS,"do_alias_get_params"},

 {NULL}  /* Sentinel */
};

static PyMethodDef PartnerDataKey0_methods[] = {
	{"get_g_code", (PyCFunction)PartnerDataKey0_get_g_code, METH_NOARGS,"do_alias_get_g_code"},
	{"set_g_code", (PyCFunction)PartnerDataKey0_set_g_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_g_code"},
	{"get_code", (PyCFunction)PartnerDataKey0_get_code, METH_NOARGS,"do_alias_get_code"},
	{"set_code", (PyCFunction)PartnerDataKey0_set_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_code"},
	{"get_type", (PyCFunction)PartnerDataKey0_get_type, METH_NOARGS,"do_alias_get_type"},
	{"set_type", (PyCFunction)PartnerDataKey0_set_type, METH_VARARGS|METH_KEYWORDS,"do_alias_get_type"},
	{"get_number", (PyCFunction)PartnerDataKey0_get_number, METH_NOARGS,"do_alias_get_number"},
	{"set_number", (PyCFunction)PartnerDataKey0_set_number, METH_VARARGS|METH_KEYWORDS,"do_alias_get_number"},

 {NULL}  /* Sentinel */
};

static PyMethodDef PartnerDataKey1_methods[] = {
	{"get_type", (PyCFunction)PartnerDataKey1_get_type, METH_NOARGS,"do_alias_get_type"},
	{"set_type", (PyCFunction)PartnerDataKey1_set_type, METH_VARARGS|METH_KEYWORDS,"do_alias_get_type"},
	{"get_g_code", (PyCFunction)PartnerDataKey1_get_g_code, METH_NOARGS,"do_alias_get_g_code"},
	{"set_g_code", (PyCFunction)PartnerDataKey1_set_g_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_g_code"},
	{"get_code", (PyCFunction)PartnerDataKey1_get_code, METH_NOARGS,"do_alias_get_code"},
	{"set_code", (PyCFunction)PartnerDataKey1_set_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_code"},

 {NULL}  /* Sentinel */
};

static PyMethodDef PartnerView_methods[] = {
	{"get_type", (PyCFunction)PartnerView_get_type, METH_NOARGS,"do_alias_get_type"},
	{"set_type", (PyCFunction)PartnerView_set_type, METH_VARARGS|METH_KEYWORDS,"do_alias_get_type"},
	{"get_sort", (PyCFunction)PartnerView_get_sort, METH_NOARGS,"do_alias_get_sort"},
	{"set_sort", (PyCFunction)PartnerView_set_sort, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sort"},
	{"get_g_code", (PyCFunction)PartnerView_get_g_code, METH_NOARGS,"do_alias_get_g_code"},
	{"set_g_code", (PyCFunction)PartnerView_set_g_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_g_code"},
	{"get_code", (PyCFunction)PartnerView_get_code, METH_NOARGS,"do_alias_get_code"},
	{"set_code", (PyCFunction)PartnerView_set_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_code"},
	{"get_type_par", (PyCFunction)PartnerView_get_type_par, METH_NOARGS,"do_alias_get_type_par"},
	{"set_type_par", (PyCFunction)PartnerView_set_type_par, METH_VARARGS|METH_KEYWORDS,"do_alias_get_type_par"},

 {NULL}  /* Sentinel */
};

static PyMethodDef PartnerViewKey0_methods[] = {
	{"get_type", (PyCFunction)PartnerViewKey0_get_type, METH_NOARGS,"do_alias_get_type"},
	{"set_type", (PyCFunction)PartnerViewKey0_set_type, METH_VARARGS|METH_KEYWORDS,"do_alias_get_type"},
	{"get_sort", (PyCFunction)PartnerViewKey0_get_sort, METH_NOARGS,"do_alias_get_sort"},
	{"set_sort", (PyCFunction)PartnerViewKey0_set_sort, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sort"},
	{"get_g_code", (PyCFunction)PartnerViewKey0_get_g_code, METH_NOARGS,"do_alias_get_g_code"},
	{"set_g_code", (PyCFunction)PartnerViewKey0_set_g_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_g_code"},
	{"get_code", (PyCFunction)PartnerViewKey0_get_code, METH_NOARGS,"do_alias_get_code"},
	{"set_code", (PyCFunction)PartnerViewKey0_set_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_code"},

 {NULL}  /* Sentinel */
};

static PyMethodDef PartnerViewKey1_methods[] = {
	{"get_g_code", (PyCFunction)PartnerViewKey1_get_g_code, METH_NOARGS,"do_alias_get_g_code"},
	{"set_g_code", (PyCFunction)PartnerViewKey1_set_g_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_g_code"},
	{"get_code", (PyCFunction)PartnerViewKey1_get_code, METH_NOARGS,"do_alias_get_code"},
	{"set_code", (PyCFunction)PartnerViewKey1_set_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_code"},
	{"get_type", (PyCFunction)PartnerViewKey1_get_type, METH_NOARGS,"do_alias_get_type"},
	{"set_type", (PyCFunction)PartnerViewKey1_set_type, METH_VARARGS|METH_KEYWORDS,"do_alias_get_type"},

 {NULL}  /* Sentinel */
};

static PyMethodDef Sklad_methods[] = {
	{"get_code", (PyCFunction)Sklad_get_code, METH_NOARGS,"do_alias_get_code"},
	{"set_code", (PyCFunction)Sklad_set_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_code"},
	{"get_name", (PyCFunction)Sklad_get_name, METH_NOARGS,"do_alias_get_name"},
	{"set_name", (PyCFunction)Sklad_set_name, METH_VARARGS|METH_KEYWORDS,"do_alias_get_name"},
	{"get_type_", (PyCFunction)Sklad_get_type_, METH_NOARGS,"do_alias_get_type_"},
	{"set_type_", (PyCFunction)Sklad_set_type_, METH_VARARGS|METH_KEYWORDS,"do_alias_get_type_"},
	{"get_p_g_code", (PyCFunction)Sklad_get_p_g_code, METH_NOARGS,"do_alias_get_p_g_code"},
	{"set_p_g_code", (PyCFunction)Sklad_set_p_g_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_p_g_code"},
	{"get_p_code", (PyCFunction)Sklad_get_p_code, METH_NOARGS,"do_alias_get_p_code"},
	{"set_p_code", (PyCFunction)Sklad_set_p_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_p_code"},
	{"get_otdel", (PyCFunction)Sklad_get_otdel, METH_NOARGS,"do_alias_get_otdel"},
	{"set_otdel", (PyCFunction)Sklad_set_otdel, METH_VARARGS|METH_KEYWORDS,"do_alias_get_otdel"},
	{"get_mol", (PyCFunction)Sklad_get_mol, METH_NOARGS,"do_alias_get_mol"},
	{"set_mol", (PyCFunction)Sklad_set_mol, METH_VARARGS|METH_KEYWORDS,"do_alias_get_mol"},
	{"get_mol1", (PyCFunction)Sklad_get_mol1, METH_NOARGS,"do_alias_get_mol1"},
	{"set_mol1", (PyCFunction)Sklad_set_mol1, METH_VARARGS|METH_KEYWORDS,"do_alias_get_mol1"},
	{"get_PASSWD", (PyCFunction)Sklad_get_PASSWD, METH_NOARGS,"do_alias_get_PASSWD"},
	{"set_PASSWD", (PyCFunction)Sklad_set_PASSWD, METH_VARARGS|METH_KEYWORDS,"do_alias_get_PASSWD"},
	{"get_TOWAR", (PyCFunction)Sklad_get_TOWAR, METH_NOARGS,"do_alias_get_TOWAR"},
	{"set_TOWAR", (PyCFunction)Sklad_set_TOWAR, METH_VARARGS|METH_KEYWORDS,"do_alias_get_TOWAR"},
	{"get_TOW_PREFIX", (PyCFunction)Sklad_get_TOW_PREFIX, METH_NOARGS,"do_alias_get_TOW_PREFIX"},
	{"set_TOW_PREFIX", (PyCFunction)Sklad_set_TOW_PREFIX, METH_VARARGS|METH_KEYWORDS,"do_alias_get_TOW_PREFIX"},
	{"get_P_ORDER", (PyCFunction)Sklad_get_P_ORDER, METH_NOARGS,"do_alias_get_P_ORDER"},
	{"set_P_ORDER", (PyCFunction)Sklad_set_P_ORDER, METH_VARARGS|METH_KEYWORDS,"do_alias_get_P_ORDER"},
	{"get_P_OPER", (PyCFunction)Sklad_get_P_OPER, METH_NOARGS,"do_alias_get_P_OPER"},
	{"set_P_OPER", (PyCFunction)Sklad_set_P_OPER, METH_VARARGS|METH_KEYWORDS,"do_alias_get_P_OPER"},
	{"get_P_DATA", (PyCFunction)Sklad_get_P_DATA, METH_NOARGS,"do_alias_get_P_DATA"},
	{"set_P_DATA", (PyCFunction)Sklad_set_P_DATA, METH_VARARGS|METH_KEYWORDS,"do_alias_get_P_DATA"},
	{"get_P_REGION", (PyCFunction)Sklad_get_P_REGION, METH_NOARGS,"do_alias_get_P_REGION"},
	{"set_P_REGION", (PyCFunction)Sklad_set_P_REGION, METH_VARARGS|METH_KEYWORDS,"do_alias_get_P_REGION"},
	{"get_P_PARTNER", (PyCFunction)Sklad_get_P_PARTNER, METH_NOARGS,"do_alias_get_P_PARTNER"},
	{"set_P_PARTNER", (PyCFunction)Sklad_set_P_PARTNER, METH_VARARGS|METH_KEYWORDS,"do_alias_get_P_PARTNER"},
	{"get_R_ORDER", (PyCFunction)Sklad_get_R_ORDER, METH_NOARGS,"do_alias_get_R_ORDER"},
	{"set_R_ORDER", (PyCFunction)Sklad_set_R_ORDER, METH_VARARGS|METH_KEYWORDS,"do_alias_get_R_ORDER"},
	{"get_R_OPER", (PyCFunction)Sklad_get_R_OPER, METH_NOARGS,"do_alias_get_R_OPER"},
	{"set_R_OPER", (PyCFunction)Sklad_set_R_OPER, METH_VARARGS|METH_KEYWORDS,"do_alias_get_R_OPER"},
	{"get_R_DATA", (PyCFunction)Sklad_get_R_DATA, METH_NOARGS,"do_alias_get_R_DATA"},
	{"set_R_DATA", (PyCFunction)Sklad_set_R_DATA, METH_VARARGS|METH_KEYWORDS,"do_alias_get_R_DATA"},
	{"get_R_REGION", (PyCFunction)Sklad_get_R_REGION, METH_NOARGS,"do_alias_get_R_REGION"},
	{"set_R_REGION", (PyCFunction)Sklad_set_R_REGION, METH_VARARGS|METH_KEYWORDS,"do_alias_get_R_REGION"},
	{"get_R_PARTNER", (PyCFunction)Sklad_get_R_PARTNER, METH_NOARGS,"do_alias_get_R_PARTNER"},
	{"set_R_PARTNER", (PyCFunction)Sklad_set_R_PARTNER, METH_VARARGS|METH_KEYWORDS,"do_alias_get_R_PARTNER"},
	{"get_SUM", (PyCFunction)Sklad_get_SUM, METH_NOARGS,"do_alias_get_SUM"},
	{"set_SUM", (PyCFunction)Sklad_set_SUM, METH_VARARGS|METH_KEYWORDS,"do_alias_get_SUM"},
	{"get_KOL", (PyCFunction)Sklad_get_KOL, METH_NOARGS,"do_alias_get_KOL"},
	{"set_KOL", (PyCFunction)Sklad_set_KOL, METH_VARARGS|METH_KEYWORDS,"do_alias_get_KOL"},
	{"get_CEK", (PyCFunction)Sklad_get_CEK, METH_NOARGS,"do_alias_get_CEK"},
	{"set_CEK", (PyCFunction)Sklad_set_CEK, METH_VARARGS|METH_KEYWORDS,"do_alias_get_CEK"},
	{"get_CONFIG_PR", (PyCFunction)Sklad_get_CONFIG_PR, METH_NOARGS,"do_alias_get_CONFIG_PR"},
	{"set_CONFIG_PR", (PyCFunction)Sklad_set_CONFIG_PR, METH_VARARGS|METH_KEYWORDS,"do_alias_get_CONFIG_PR"},
	{"get_CONFIG_NUM", (PyCFunction)Sklad_get_CONFIG_NUM, METH_NOARGS,"do_alias_get_CONFIG_NUM"},
	{"set_CONFIG_NUM", (PyCFunction)Sklad_set_CONFIG_NUM, METH_VARARGS|METH_KEYWORDS,"do_alias_get_CONFIG_NUM"},
	{"get_stat", (PyCFunction)Sklad_get_stat, METH_NOARGS,"do_alias_get_stat"},
	{"set_stat", (PyCFunction)Sklad_set_stat, METH_VARARGS|METH_KEYWORDS,"do_alias_get_stat"},
	{"get_params", (PyCFunction)Sklad_get_params, METH_NOARGS,"do_alias_get_params"},
	{"set_params", (PyCFunction)Sklad_set_params, METH_VARARGS|METH_KEYWORDS,"do_alias_get_params"},

 {NULL}  /* Sentinel */
};

static PyMethodDef SkladKey0_methods[] = {
	{"get_code", (PyCFunction)SkladKey0_get_code, METH_NOARGS,"do_alias_get_code"},
	{"set_code", (PyCFunction)SkladKey0_set_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_code"},

 {NULL}  /* Sentinel */
};

static PyMethodDef DocumentProw_methods[] = {
	{"get_dtype", (PyCFunction)DocumentProw_get_dtype, METH_NOARGS,"do_alias_get_dtype"},
	{"set_dtype", (PyCFunction)DocumentProw_set_dtype, METH_VARARGS|METH_KEYWORDS,"do_alias_get_dtype"},
	{"get_dtype1", (PyCFunction)DocumentProw_get_dtype1, METH_NOARGS,"do_alias_get_dtype1"},
	{"set_dtype1", (PyCFunction)DocumentProw_set_dtype1, METH_VARARGS|METH_KEYWORDS,"do_alias_get_dtype1"},
	{"get_number", (PyCFunction)DocumentProw_get_number, METH_NOARGS,"do_alias_get_number"},
	{"set_number", (PyCFunction)DocumentProw_set_number, METH_VARARGS|METH_KEYWORDS,"do_alias_get_number"},
	{"get_params", (PyCFunction)DocumentProw_get_params, METH_NOARGS,"do_alias_get_params"},
	{"set_params", (PyCFunction)DocumentProw_set_params, METH_VARARGS|METH_KEYWORDS,"do_alias_get_params"},

 {NULL}  /* Sentinel */
};

static PyMethodDef DocumentProwKey0_methods[] = {
	{"get_dtype", (PyCFunction)DocumentProwKey0_get_dtype, METH_NOARGS,"do_alias_get_dtype"},
	{"set_dtype", (PyCFunction)DocumentProwKey0_set_dtype, METH_VARARGS|METH_KEYWORDS,"do_alias_get_dtype"},
	{"get_dtype1", (PyCFunction)DocumentProwKey0_get_dtype1, METH_NOARGS,"do_alias_get_dtype1"},
	{"set_dtype1", (PyCFunction)DocumentProwKey0_set_dtype1, METH_VARARGS|METH_KEYWORDS,"do_alias_get_dtype1"},
	{"get_number", (PyCFunction)DocumentProwKey0_get_number, METH_NOARGS,"do_alias_get_number"},
	{"set_number", (PyCFunction)DocumentProwKey0_set_number, METH_VARARGS|METH_KEYWORDS,"do_alias_get_number"},

 {NULL}  /* Sentinel */
};

static PyMethodDef DocumentType_methods[] = {
	{"get_dtype", (PyCFunction)DocumentType_get_dtype, METH_NOARGS,"do_alias_get_dtype"},
	{"set_dtype", (PyCFunction)DocumentType_set_dtype, METH_VARARGS|METH_KEYWORDS,"do_alias_get_dtype"},
	{"get_dtype1", (PyCFunction)DocumentType_get_dtype1, METH_NOARGS,"do_alias_get_dtype1"},
	{"set_dtype1", (PyCFunction)DocumentType_set_dtype1, METH_VARARGS|METH_KEYWORDS,"do_alias_get_dtype1"},
	{"get_name", (PyCFunction)DocumentType_get_name, METH_NOARGS,"do_alias_get_name"},
	{"set_name", (PyCFunction)DocumentType_set_name, METH_VARARGS|METH_KEYWORDS,"do_alias_get_name"},
	{"get_sname", (PyCFunction)DocumentType_get_sname, METH_NOARGS,"do_alias_get_sname"},
	{"set_sname", (PyCFunction)DocumentType_set_sname, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sname"},
	{"get_prices", (PyCFunction)DocumentType_get_prices, METH_NOARGS,"do_alias_get_prices"},
	{"set_prices", (PyCFunction)DocumentType_set_prices, METH_VARARGS|METH_KEYWORDS,"do_alias_get_prices"},
	{"get_print", (PyCFunction)DocumentType_get_print, METH_NOARGS,"do_alias_get_print"},
	{"set_print", (PyCFunction)DocumentType_set_print, METH_VARARGS|METH_KEYWORDS,"do_alias_get_print"},
	{"get_form", (PyCFunction)DocumentType_get_form, METH_NOARGS,"do_alias_get_form"},
	{"set_form", (PyCFunction)DocumentType_set_form, METH_VARARGS|METH_KEYWORDS,"do_alias_get_form"},
	{"get_accept_cmd", (PyCFunction)DocumentType_get_accept_cmd, METH_NOARGS,"do_alias_get_accept_cmd"},
	{"set_accept_cmd", (PyCFunction)DocumentType_set_accept_cmd, METH_VARARGS|METH_KEYWORDS,"do_alias_get_accept_cmd"},
	{"get_deaccept_cmd", (PyCFunction)DocumentType_get_deaccept_cmd, METH_NOARGS,"do_alias_get_deaccept_cmd"},
	{"set_deaccept_cmd", (PyCFunction)DocumentType_set_deaccept_cmd, METH_VARARGS|METH_KEYWORDS,"do_alias_get_deaccept_cmd"},
	{"get_c_dtypes", (PyCFunction)DocumentType_get_c_dtypes, METH_NOARGS,"do_alias_get_c_dtypes"},
	{"set_c_dtypes", (PyCFunction)DocumentType_set_c_dtypes, METH_VARARGS|METH_KEYWORDS,"do_alias_get_c_dtypes"},
	{"get_d_param", (PyCFunction)DocumentType_get_d_param, METH_NOARGS,"do_alias_get_d_param"},
	{"set_d_param", (PyCFunction)DocumentType_set_d_param, METH_VARARGS|METH_KEYWORDS,"do_alias_get_d_param"},
	{"get_params", (PyCFunction)DocumentType_get_params, METH_NOARGS,"do_alias_get_params"},
	{"set_params", (PyCFunction)DocumentType_set_params, METH_VARARGS|METH_KEYWORDS,"do_alias_get_params"},

 {NULL}  /* Sentinel */
};

static PyMethodDef DocumentTypeKey0_methods[] = {
	{"get_dtype", (PyCFunction)DocumentTypeKey0_get_dtype, METH_NOARGS,"do_alias_get_dtype"},
	{"set_dtype", (PyCFunction)DocumentTypeKey0_set_dtype, METH_VARARGS|METH_KEYWORDS,"do_alias_get_dtype"},
	{"get_dtype1", (PyCFunction)DocumentTypeKey0_get_dtype1, METH_NOARGS,"do_alias_get_dtype1"},
	{"set_dtype1", (PyCFunction)DocumentTypeKey0_set_dtype1, METH_VARARGS|METH_KEYWORDS,"do_alias_get_dtype1"},

 {NULL}  /* Sentinel */
};

static PyMethodDef Stock_methods[] = {
	{"get_sklad", (PyCFunction)Stock_get_sklad, METH_NOARGS,"do_alias_get_sklad"},
	{"set_sklad", (PyCFunction)Stock_set_sklad, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sklad"},
	{"get_code", (PyCFunction)Stock_get_code, METH_NOARGS,"do_alias_get_code"},
	{"set_code", (PyCFunction)Stock_set_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_code"},
	{"get_quant", (PyCFunction)Stock_get_quant, METH_NOARGS,"do_alias_get_quant"},
	{"set_quant", (PyCFunction)Stock_set_quant, METH_VARARGS|METH_KEYWORDS,"do_alias_get_quant"},
	{"get_nquant", (PyCFunction)Stock_get_nquant, METH_NOARGS,"do_alias_get_nquant"},
	{"set_nquant", (PyCFunction)Stock_set_nquant, METH_VARARGS|METH_KEYWORDS,"do_alias_get_nquant"},

 {NULL}  /* Sentinel */
};

static PyMethodDef StockKey0_methods[] = {
	{"get_sklad", (PyCFunction)StockKey0_get_sklad, METH_NOARGS,"do_alias_get_sklad"},
	{"set_sklad", (PyCFunction)StockKey0_set_sklad, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sklad"},
	{"get_code", (PyCFunction)StockKey0_get_code, METH_NOARGS,"do_alias_get_code"},
	{"set_code", (PyCFunction)StockKey0_set_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_code"},

 {NULL}  /* Sentinel */
};

static PyMethodDef StockKey1_methods[] = {
	{"get_code", (PyCFunction)StockKey1_get_code, METH_NOARGS,"do_alias_get_code"},
	{"set_code", (PyCFunction)StockKey1_set_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_code"},

 {NULL}  /* Sentinel */
};

static PyMethodDef Prowod_methods[] = {
	{"get_division", (PyCFunction)Prowod_get_division, METH_NOARGS,"do_alias_get_division"},
	{"set_division", (PyCFunction)Prowod_set_division, METH_VARARGS|METH_KEYWORDS,"do_alias_get_division"},
	{"get_date", (PyCFunction)Prowod_get_date, METH_NOARGS,"do_alias_get_date"},
	{"set_date", (PyCFunction)Prowod_set_date, METH_VARARGS|METH_KEYWORDS,"do_alias_get_date"},
	{"get_debit_account", (PyCFunction)Prowod_get_debit_account, METH_NOARGS,"do_alias_get_debit_account"},
	{"set_debit_account", (PyCFunction)Prowod_set_debit_account, METH_VARARGS|METH_KEYWORDS,"do_alias_get_debit_account"},
	{"get_debit_subaccount", (PyCFunction)Prowod_get_debit_subaccount, METH_NOARGS,"do_alias_get_debit_subaccount"},
	{"set_debit_subaccount", (PyCFunction)Prowod_set_debit_subaccount, METH_VARARGS|METH_KEYWORDS,"do_alias_get_debit_subaccount"},
	{"get_debit_other", (PyCFunction)Prowod_get_debit_other, METH_NOARGS,"do_alias_get_debit_other"},
	{"set_debit_other", (PyCFunction)Prowod_set_debit_other, METH_VARARGS|METH_KEYWORDS,"do_alias_get_debit_other"},
	{"get_debit_p_g_code", (PyCFunction)Prowod_get_debit_p_g_code, METH_NOARGS,"do_alias_get_debit_p_g_code"},
	{"set_debit_p_g_code", (PyCFunction)Prowod_set_debit_p_g_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_debit_p_g_code"},
	{"get_debit_p_code", (PyCFunction)Prowod_get_debit_p_code, METH_NOARGS,"do_alias_get_debit_p_code"},
	{"set_debit_p_code", (PyCFunction)Prowod_set_debit_p_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_debit_p_code"},
	{"get_credit_account", (PyCFunction)Prowod_get_credit_account, METH_NOARGS,"do_alias_get_credit_account"},
	{"set_credit_account", (PyCFunction)Prowod_set_credit_account, METH_VARARGS|METH_KEYWORDS,"do_alias_get_credit_account"},
	{"get_credit_subaccount", (PyCFunction)Prowod_get_credit_subaccount, METH_NOARGS,"do_alias_get_credit_subaccount"},
	{"set_credit_subaccount", (PyCFunction)Prowod_set_credit_subaccount, METH_VARARGS|METH_KEYWORDS,"do_alias_get_credit_subaccount"},
	{"get_credit_other", (PyCFunction)Prowod_get_credit_other, METH_NOARGS,"do_alias_get_credit_other"},
	{"set_credit_other", (PyCFunction)Prowod_set_credit_other, METH_VARARGS|METH_KEYWORDS,"do_alias_get_credit_other"},
	{"get_credit_p_g_code", (PyCFunction)Prowod_get_credit_p_g_code, METH_NOARGS,"do_alias_get_credit_p_g_code"},
	{"set_credit_p_g_code", (PyCFunction)Prowod_set_credit_p_g_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_credit_p_g_code"},
	{"get_credit_p_code", (PyCFunction)Prowod_get_credit_p_code, METH_NOARGS,"do_alias_get_credit_p_code"},
	{"set_credit_p_code", (PyCFunction)Prowod_set_credit_p_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_credit_p_code"},
	{"get_valuta", (PyCFunction)Prowod_get_valuta, METH_NOARGS,"do_alias_get_valuta"},
	{"set_valuta", (PyCFunction)Prowod_set_valuta, METH_VARARGS|METH_KEYWORDS,"do_alias_get_valuta"},
	{"get_sum", (PyCFunction)Prowod_get_sum, METH_NOARGS,"do_alias_get_sum"},
	{"set_sum", (PyCFunction)Prowod_set_sum, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sum"},
	{"get_sums", (PyCFunction)Prowod_get_sums, METH_NOARGS,"do_alias_get_sums"},
	{"set_sums", (PyCFunction)Prowod_set_sums, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sums"},
	{"get_type_doc", (PyCFunction)Prowod_get_type_doc, METH_NOARGS,"do_alias_get_type_doc"},
	{"set_type_doc", (PyCFunction)Prowod_set_type_doc, METH_VARARGS|METH_KEYWORDS,"do_alias_get_type_doc"},
	{"get_type_oper", (PyCFunction)Prowod_get_type_oper, METH_NOARGS,"do_alias_get_type_oper"},
	{"set_type_oper", (PyCFunction)Prowod_set_type_oper, METH_VARARGS|METH_KEYWORDS,"do_alias_get_type_oper"},
	{"get_oper", (PyCFunction)Prowod_get_oper, METH_NOARGS,"do_alias_get_oper"},
	{"set_oper", (PyCFunction)Prowod_set_oper, METH_VARARGS|METH_KEYWORDS,"do_alias_get_oper"},
	{"get_document", (PyCFunction)Prowod_get_document, METH_NOARGS,"do_alias_get_document"},
	{"set_document", (PyCFunction)Prowod_set_document, METH_VARARGS|METH_KEYWORDS,"do_alias_get_document"},
	{"get_pach", (PyCFunction)Prowod_get_pach, METH_NOARGS,"do_alias_get_pach"},
	{"set_pach", (PyCFunction)Prowod_set_pach, METH_VARARGS|METH_KEYWORDS,"do_alias_get_pach"},
	{"get_params", (PyCFunction)Prowod_get_params, METH_NOARGS,"do_alias_get_params"},
	{"set_params", (PyCFunction)Prowod_set_params, METH_VARARGS|METH_KEYWORDS,"do_alias_get_params"},

 {NULL}  /* Sentinel */
};

static PyMethodDef ProwodKey0_methods[] = {
	{"get_division", (PyCFunction)ProwodKey0_get_division, METH_NOARGS,"do_alias_get_division"},
	{"set_division", (PyCFunction)ProwodKey0_set_division, METH_VARARGS|METH_KEYWORDS,"do_alias_get_division"},
	{"get_date", (PyCFunction)ProwodKey0_get_date, METH_NOARGS,"do_alias_get_date"},
	{"set_date", (PyCFunction)ProwodKey0_set_date, METH_VARARGS|METH_KEYWORDS,"do_alias_get_date"},
	{"get_type_doc", (PyCFunction)ProwodKey0_get_type_doc, METH_NOARGS,"do_alias_get_type_doc"},
	{"set_type_doc", (PyCFunction)ProwodKey0_set_type_doc, METH_VARARGS|METH_KEYWORDS,"do_alias_get_type_doc"},
	{"get_document", (PyCFunction)ProwodKey0_get_document, METH_NOARGS,"do_alias_get_document"},
	{"set_document", (PyCFunction)ProwodKey0_set_document, METH_VARARGS|METH_KEYWORDS,"do_alias_get_document"},

 {NULL}  /* Sentinel */
};

static PyMethodDef ProwodKey1_methods[] = {
	{"get_division", (PyCFunction)ProwodKey1_get_division, METH_NOARGS,"do_alias_get_division"},
	{"set_division", (PyCFunction)ProwodKey1_set_division, METH_VARARGS|METH_KEYWORDS,"do_alias_get_division"},
	{"get_type_doc", (PyCFunction)ProwodKey1_get_type_doc, METH_NOARGS,"do_alias_get_type_doc"},
	{"set_type_doc", (PyCFunction)ProwodKey1_set_type_doc, METH_VARARGS|METH_KEYWORDS,"do_alias_get_type_doc"},
	{"get_date", (PyCFunction)ProwodKey1_get_date, METH_NOARGS,"do_alias_get_date"},
	{"set_date", (PyCFunction)ProwodKey1_set_date, METH_VARARGS|METH_KEYWORDS,"do_alias_get_date"},
	{"get_document", (PyCFunction)ProwodKey1_get_document, METH_NOARGS,"do_alias_get_document"},
	{"set_document", (PyCFunction)ProwodKey1_set_document, METH_VARARGS|METH_KEYWORDS,"do_alias_get_document"},

 {NULL}  /* Sentinel */
};

static PyMethodDef ProwodKey2_methods[] = {
	{"get_division", (PyCFunction)ProwodKey2_get_division, METH_NOARGS,"do_alias_get_division"},
	{"set_division", (PyCFunction)ProwodKey2_set_division, METH_VARARGS|METH_KEYWORDS,"do_alias_get_division"},
	{"get_debit_account", (PyCFunction)ProwodKey2_get_debit_account, METH_NOARGS,"do_alias_get_debit_account"},
	{"set_debit_account", (PyCFunction)ProwodKey2_set_debit_account, METH_VARARGS|METH_KEYWORDS,"do_alias_get_debit_account"},
	{"get_debit_subaccount", (PyCFunction)ProwodKey2_get_debit_subaccount, METH_NOARGS,"do_alias_get_debit_subaccount"},
	{"set_debit_subaccount", (PyCFunction)ProwodKey2_set_debit_subaccount, METH_VARARGS|METH_KEYWORDS,"do_alias_get_debit_subaccount"},
	{"get_debit_p_g_code", (PyCFunction)ProwodKey2_get_debit_p_g_code, METH_NOARGS,"do_alias_get_debit_p_g_code"},
	{"set_debit_p_g_code", (PyCFunction)ProwodKey2_set_debit_p_g_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_debit_p_g_code"},
	{"get_debit_p_code", (PyCFunction)ProwodKey2_get_debit_p_code, METH_NOARGS,"do_alias_get_debit_p_code"},
	{"set_debit_p_code", (PyCFunction)ProwodKey2_set_debit_p_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_debit_p_code"},
	{"get_debit_other", (PyCFunction)ProwodKey2_get_debit_other, METH_NOARGS,"do_alias_get_debit_other"},
	{"set_debit_other", (PyCFunction)ProwodKey2_set_debit_other, METH_VARARGS|METH_KEYWORDS,"do_alias_get_debit_other"},
	{"get_date", (PyCFunction)ProwodKey2_get_date, METH_NOARGS,"do_alias_get_date"},
	{"set_date", (PyCFunction)ProwodKey2_set_date, METH_VARARGS|METH_KEYWORDS,"do_alias_get_date"},

 {NULL}  /* Sentinel */
};

static PyMethodDef ProwodKey3_methods[] = {
	{"get_division", (PyCFunction)ProwodKey3_get_division, METH_NOARGS,"do_alias_get_division"},
	{"set_division", (PyCFunction)ProwodKey3_set_division, METH_VARARGS|METH_KEYWORDS,"do_alias_get_division"},
	{"get_credit_account", (PyCFunction)ProwodKey3_get_credit_account, METH_NOARGS,"do_alias_get_credit_account"},
	{"set_credit_account", (PyCFunction)ProwodKey3_set_credit_account, METH_VARARGS|METH_KEYWORDS,"do_alias_get_credit_account"},
	{"get_credit_subaccount", (PyCFunction)ProwodKey3_get_credit_subaccount, METH_NOARGS,"do_alias_get_credit_subaccount"},
	{"set_credit_subaccount", (PyCFunction)ProwodKey3_set_credit_subaccount, METH_VARARGS|METH_KEYWORDS,"do_alias_get_credit_subaccount"},
	{"get_credit_p_g_code", (PyCFunction)ProwodKey3_get_credit_p_g_code, METH_NOARGS,"do_alias_get_credit_p_g_code"},
	{"set_credit_p_g_code", (PyCFunction)ProwodKey3_set_credit_p_g_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_credit_p_g_code"},
	{"get_credit_p_code", (PyCFunction)ProwodKey3_get_credit_p_code, METH_NOARGS,"do_alias_get_credit_p_code"},
	{"set_credit_p_code", (PyCFunction)ProwodKey3_set_credit_p_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_credit_p_code"},
	{"get_credit_other", (PyCFunction)ProwodKey3_get_credit_other, METH_NOARGS,"do_alias_get_credit_other"},
	{"set_credit_other", (PyCFunction)ProwodKey3_set_credit_other, METH_VARARGS|METH_KEYWORDS,"do_alias_get_credit_other"},
	{"get_date", (PyCFunction)ProwodKey3_get_date, METH_NOARGS,"do_alias_get_date"},
	{"set_date", (PyCFunction)ProwodKey3_set_date, METH_VARARGS|METH_KEYWORDS,"do_alias_get_date"},

 {NULL}  /* Sentinel */
};

static PyMethodDef ProwodKey4_methods[] = {
	{"get_division", (PyCFunction)ProwodKey4_get_division, METH_NOARGS,"do_alias_get_division"},
	{"set_division", (PyCFunction)ProwodKey4_set_division, METH_VARARGS|METH_KEYWORDS,"do_alias_get_division"},
	{"get_document", (PyCFunction)ProwodKey4_get_document, METH_NOARGS,"do_alias_get_document"},
	{"set_document", (PyCFunction)ProwodKey4_set_document, METH_VARARGS|METH_KEYWORDS,"do_alias_get_document"},
	{"get_date", (PyCFunction)ProwodKey4_get_date, METH_NOARGS,"do_alias_get_date"},
	{"set_date", (PyCFunction)ProwodKey4_set_date, METH_VARARGS|METH_KEYWORDS,"do_alias_get_date"},

 {NULL}  /* Sentinel */
};

static PyMethodDef Prowod2_methods[] = {
	{"get_dtype", (PyCFunction)Prowod2_get_dtype, METH_NOARGS,"do_alias_get_dtype"},
	{"set_dtype", (PyCFunction)Prowod2_set_dtype, METH_VARARGS|METH_KEYWORDS,"do_alias_get_dtype"},
	{"get_sklad", (PyCFunction)Prowod2_get_sklad, METH_NOARGS,"do_alias_get_sklad"},
	{"set_sklad", (PyCFunction)Prowod2_set_sklad, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sklad"},
	{"get_document", (PyCFunction)Prowod2_get_document, METH_NOARGS,"do_alias_get_document"},
	{"set_document", (PyCFunction)Prowod2_set_document, METH_VARARGS|METH_KEYWORDS,"do_alias_get_document"},
	{"get_number", (PyCFunction)Prowod2_get_number, METH_NOARGS,"do_alias_get_number"},
	{"set_number", (PyCFunction)Prowod2_set_number, METH_VARARGS|METH_KEYWORDS,"do_alias_get_number"},
	{"get_otdel", (PyCFunction)Prowod2_get_otdel, METH_NOARGS,"do_alias_get_otdel"},
	{"set_otdel", (PyCFunction)Prowod2_set_otdel, METH_VARARGS|METH_KEYWORDS,"do_alias_get_otdel"},
	{"get_date", (PyCFunction)Prowod2_get_date, METH_NOARGS,"do_alias_get_date"},
	{"set_date", (PyCFunction)Prowod2_set_date, METH_VARARGS|METH_KEYWORDS,"do_alias_get_date"},
	{"get_time", (PyCFunction)Prowod2_get_time, METH_NOARGS,"do_alias_get_time"},
	{"set_time", (PyCFunction)Prowod2_set_time, METH_VARARGS|METH_KEYWORDS,"do_alias_get_time"},
	{"get_saldo_d", (PyCFunction)Prowod2_get_saldo_d, METH_NOARGS,"do_alias_get_saldo_d"},
	{"set_saldo_d", (PyCFunction)Prowod2_set_saldo_d, METH_VARARGS|METH_KEYWORDS,"do_alias_get_saldo_d"},
	{"get_saldo_k", (PyCFunction)Prowod2_get_saldo_k, METH_NOARGS,"do_alias_get_saldo_k"},
	{"set_saldo_k", (PyCFunction)Prowod2_set_saldo_k, METH_VARARGS|METH_KEYWORDS,"do_alias_get_saldo_k"},
	{"get_nsum", (PyCFunction)Prowod2_get_nsum, METH_NOARGS,"do_alias_get_nsum"},
	{"set_nsum", (PyCFunction)Prowod2_set_nsum, METH_VARARGS|METH_KEYWORDS,"do_alias_get_nsum"},
	{"get_sum", (PyCFunction)Prowod2_get_sum, METH_NOARGS,"do_alias_get_sum"},
	{"set_sum", (PyCFunction)Prowod2_set_sum, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sum"},
	{"get_params", (PyCFunction)Prowod2_get_params, METH_NOARGS,"do_alias_get_params"},
	{"set_params", (PyCFunction)Prowod2_set_params, METH_VARARGS|METH_KEYWORDS,"do_alias_get_params"},

 {NULL}  /* Sentinel */
};

static PyMethodDef Prowod2Key0_methods[] = {
	{"get_dtype", (PyCFunction)Prowod2Key0_get_dtype, METH_NOARGS,"do_alias_get_dtype"},
	{"set_dtype", (PyCFunction)Prowod2Key0_set_dtype, METH_VARARGS|METH_KEYWORDS,"do_alias_get_dtype"},
	{"get_sklad", (PyCFunction)Prowod2Key0_get_sklad, METH_NOARGS,"do_alias_get_sklad"},
	{"set_sklad", (PyCFunction)Prowod2Key0_set_sklad, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sklad"},
	{"get_document", (PyCFunction)Prowod2Key0_get_document, METH_NOARGS,"do_alias_get_document"},
	{"set_document", (PyCFunction)Prowod2Key0_set_document, METH_VARARGS|METH_KEYWORDS,"do_alias_get_document"},
	{"get_number", (PyCFunction)Prowod2Key0_get_number, METH_NOARGS,"do_alias_get_number"},
	{"set_number", (PyCFunction)Prowod2Key0_set_number, METH_VARARGS|METH_KEYWORDS,"do_alias_get_number"},
	{"get_otdel", (PyCFunction)Prowod2Key0_get_otdel, METH_NOARGS,"do_alias_get_otdel"},
	{"set_otdel", (PyCFunction)Prowod2Key0_set_otdel, METH_VARARGS|METH_KEYWORDS,"do_alias_get_otdel"},

 {NULL}  /* Sentinel */
};

static PyMethodDef Prowod2Key1_methods[] = {
	{"get_otdel", (PyCFunction)Prowod2Key1_get_otdel, METH_NOARGS,"do_alias_get_otdel"},
	{"set_otdel", (PyCFunction)Prowod2Key1_set_otdel, METH_VARARGS|METH_KEYWORDS,"do_alias_get_otdel"},
	{"get_date", (PyCFunction)Prowod2Key1_get_date, METH_NOARGS,"do_alias_get_date"},
	{"set_date", (PyCFunction)Prowod2Key1_set_date, METH_VARARGS|METH_KEYWORDS,"do_alias_get_date"},
	{"get_time", (PyCFunction)Prowod2Key1_get_time, METH_NOARGS,"do_alias_get_time"},
	{"set_time", (PyCFunction)Prowod2Key1_set_time, METH_VARARGS|METH_KEYWORDS,"do_alias_get_time"},

 {NULL}  /* Sentinel */
};

static PyMethodDef Prowod2Key2_methods[] = {
	{"get_otdel", (PyCFunction)Prowod2Key2_get_otdel, METH_NOARGS,"do_alias_get_otdel"},
	{"set_otdel", (PyCFunction)Prowod2Key2_set_otdel, METH_VARARGS|METH_KEYWORDS,"do_alias_get_otdel"},
	{"get_saldo_d", (PyCFunction)Prowod2Key2_get_saldo_d, METH_NOARGS,"do_alias_get_saldo_d"},
	{"set_saldo_d", (PyCFunction)Prowod2Key2_set_saldo_d, METH_VARARGS|METH_KEYWORDS,"do_alias_get_saldo_d"},
	{"get_date", (PyCFunction)Prowod2Key2_get_date, METH_NOARGS,"do_alias_get_date"},
	{"set_date", (PyCFunction)Prowod2Key2_set_date, METH_VARARGS|METH_KEYWORDS,"do_alias_get_date"},
	{"get_time", (PyCFunction)Prowod2Key2_get_time, METH_NOARGS,"do_alias_get_time"},
	{"set_time", (PyCFunction)Prowod2Key2_set_time, METH_VARARGS|METH_KEYWORDS,"do_alias_get_time"},

 {NULL}  /* Sentinel */
};

static PyMethodDef Prowod2Key3_methods[] = {
	{"get_otdel", (PyCFunction)Prowod2Key3_get_otdel, METH_NOARGS,"do_alias_get_otdel"},
	{"set_otdel", (PyCFunction)Prowod2Key3_set_otdel, METH_VARARGS|METH_KEYWORDS,"do_alias_get_otdel"},
	{"get_saldo_k", (PyCFunction)Prowod2Key3_get_saldo_k, METH_NOARGS,"do_alias_get_saldo_k"},
	{"set_saldo_k", (PyCFunction)Prowod2Key3_set_saldo_k, METH_VARARGS|METH_KEYWORDS,"do_alias_get_saldo_k"},
	{"get_date", (PyCFunction)Prowod2Key3_get_date, METH_NOARGS,"do_alias_get_date"},
	{"set_date", (PyCFunction)Prowod2Key3_set_date, METH_VARARGS|METH_KEYWORDS,"do_alias_get_date"},
	{"get_time", (PyCFunction)Prowod2Key3_get_time, METH_NOARGS,"do_alias_get_time"},
	{"set_time", (PyCFunction)Prowod2Key3_set_time, METH_VARARGS|METH_KEYWORDS,"do_alias_get_time"},

 {NULL}  /* Sentinel */
};

static PyMethodDef Saldo_methods[] = {
	{"get_division", (PyCFunction)Saldo_get_division, METH_NOARGS,"do_alias_get_division"},
	{"set_division", (PyCFunction)Saldo_set_division, METH_VARARGS|METH_KEYWORDS,"do_alias_get_division"},
	{"get_layer", (PyCFunction)Saldo_get_layer, METH_NOARGS,"do_alias_get_layer"},
	{"set_layer", (PyCFunction)Saldo_set_layer, METH_VARARGS|METH_KEYWORDS,"do_alias_get_layer"},
	{"get_account", (PyCFunction)Saldo_get_account, METH_NOARGS,"do_alias_get_account"},
	{"set_account", (PyCFunction)Saldo_set_account, METH_VARARGS|METH_KEYWORDS,"do_alias_get_account"},
	{"get_account_total", (PyCFunction)Saldo_get_account_total, METH_NOARGS,"do_alias_get_account_total"},
	{"set_account_total", (PyCFunction)Saldo_set_account_total, METH_VARARGS|METH_KEYWORDS,"do_alias_get_account_total"},
	{"get_subaccount", (PyCFunction)Saldo_get_subaccount, METH_NOARGS,"do_alias_get_subaccount"},
	{"set_subaccount", (PyCFunction)Saldo_set_subaccount, METH_VARARGS|METH_KEYWORDS,"do_alias_get_subaccount"},
	{"get_subaccount_total", (PyCFunction)Saldo_get_subaccount_total, METH_NOARGS,"do_alias_get_subaccount_total"},
	{"set_subaccount_total", (PyCFunction)Saldo_set_subaccount_total, METH_VARARGS|METH_KEYWORDS,"do_alias_get_subaccount_total"},
	{"get_p_g_code", (PyCFunction)Saldo_get_p_g_code, METH_NOARGS,"do_alias_get_p_g_code"},
	{"set_p_g_code", (PyCFunction)Saldo_set_p_g_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_p_g_code"},
	{"get_p_code", (PyCFunction)Saldo_get_p_code, METH_NOARGS,"do_alias_get_p_code"},
	{"set_p_code", (PyCFunction)Saldo_set_p_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_p_code"},
	{"get_other", (PyCFunction)Saldo_get_other, METH_NOARGS,"do_alias_get_other"},
	{"set_other", (PyCFunction)Saldo_set_other, METH_VARARGS|METH_KEYWORDS,"do_alias_get_other"},
	{"get_date_n", (PyCFunction)Saldo_get_date_n, METH_NOARGS,"do_alias_get_date_n"},
	{"set_date_n", (PyCFunction)Saldo_set_date_n, METH_VARARGS|METH_KEYWORDS,"do_alias_get_date_n"},
	{"get_date", (PyCFunction)Saldo_get_date, METH_NOARGS,"do_alias_get_date"},
	{"set_date", (PyCFunction)Saldo_set_date, METH_VARARGS|METH_KEYWORDS,"do_alias_get_date"},
	{"get_date_modify", (PyCFunction)Saldo_get_date_modify, METH_NOARGS,"do_alias_get_date_modify"},
	{"set_date_modify", (PyCFunction)Saldo_set_date_modify, METH_VARARGS|METH_KEYWORDS,"do_alias_get_date_modify"},
	{"get_time_modify", (PyCFunction)Saldo_get_time_modify, METH_NOARGS,"do_alias_get_time_modify"},
	{"set_time_modify", (PyCFunction)Saldo_set_time_modify, METH_VARARGS|METH_KEYWORDS,"do_alias_get_time_modify"},
	{"get_debit_sum", (PyCFunction)Saldo_get_debit_sum, METH_NOARGS,"do_alias_get_debit_sum"},
	{"set_debit_sum", (PyCFunction)Saldo_set_debit_sum, METH_VARARGS|METH_KEYWORDS,"do_alias_get_debit_sum"},
	{"get_credit_sum", (PyCFunction)Saldo_get_credit_sum, METH_NOARGS,"do_alias_get_credit_sum"},
	{"set_credit_sum", (PyCFunction)Saldo_set_credit_sum, METH_VARARGS|METH_KEYWORDS,"do_alias_get_credit_sum"},

 {NULL}  /* Sentinel */
};

static PyMethodDef SaldoKey0_methods[] = {
	{"get_division", (PyCFunction)SaldoKey0_get_division, METH_NOARGS,"do_alias_get_division"},
	{"set_division", (PyCFunction)SaldoKey0_set_division, METH_VARARGS|METH_KEYWORDS,"do_alias_get_division"},
	{"get_layer", (PyCFunction)SaldoKey0_get_layer, METH_NOARGS,"do_alias_get_layer"},
	{"set_layer", (PyCFunction)SaldoKey0_set_layer, METH_VARARGS|METH_KEYWORDS,"do_alias_get_layer"},
	{"get_account", (PyCFunction)SaldoKey0_get_account, METH_NOARGS,"do_alias_get_account"},
	{"set_account", (PyCFunction)SaldoKey0_set_account, METH_VARARGS|METH_KEYWORDS,"do_alias_get_account"},
	{"get_account_total", (PyCFunction)SaldoKey0_get_account_total, METH_NOARGS,"do_alias_get_account_total"},
	{"set_account_total", (PyCFunction)SaldoKey0_set_account_total, METH_VARARGS|METH_KEYWORDS,"do_alias_get_account_total"},
	{"get_subaccount", (PyCFunction)SaldoKey0_get_subaccount, METH_NOARGS,"do_alias_get_subaccount"},
	{"set_subaccount", (PyCFunction)SaldoKey0_set_subaccount, METH_VARARGS|METH_KEYWORDS,"do_alias_get_subaccount"},
	{"get_subaccount_total", (PyCFunction)SaldoKey0_get_subaccount_total, METH_NOARGS,"do_alias_get_subaccount_total"},
	{"set_subaccount_total", (PyCFunction)SaldoKey0_set_subaccount_total, METH_VARARGS|METH_KEYWORDS,"do_alias_get_subaccount_total"},
	{"get_p_g_code", (PyCFunction)SaldoKey0_get_p_g_code, METH_NOARGS,"do_alias_get_p_g_code"},
	{"set_p_g_code", (PyCFunction)SaldoKey0_set_p_g_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_p_g_code"},
	{"get_p_code", (PyCFunction)SaldoKey0_get_p_code, METH_NOARGS,"do_alias_get_p_code"},
	{"set_p_code", (PyCFunction)SaldoKey0_set_p_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_p_code"},
	{"get_other", (PyCFunction)SaldoKey0_get_other, METH_NOARGS,"do_alias_get_other"},
	{"set_other", (PyCFunction)SaldoKey0_set_other, METH_VARARGS|METH_KEYWORDS,"do_alias_get_other"},

 {NULL}  /* Sentinel */
};

static PyMethodDef Protocol_methods[] = {
	{"get_objectName", (PyCFunction)Protocol_get_objectName, METH_NOARGS,"do_alias_get_objectName"},
	{"set_objectName", (PyCFunction)Protocol_set_objectName, METH_VARARGS|METH_KEYWORDS,"do_alias_get_objectName"},
	{"get_userName", (PyCFunction)Protocol_get_userName, METH_NOARGS,"do_alias_get_userName"},
	{"set_userName", (PyCFunction)Protocol_set_userName, METH_VARARGS|METH_KEYWORDS,"do_alias_get_userName"},
	{"get_date", (PyCFunction)Protocol_get_date, METH_NOARGS,"do_alias_get_date"},
	{"set_date", (PyCFunction)Protocol_set_date, METH_VARARGS|METH_KEYWORDS,"do_alias_get_date"},
	{"get_time", (PyCFunction)Protocol_get_time, METH_NOARGS,"do_alias_get_time"},
	{"set_time", (PyCFunction)Protocol_set_time, METH_VARARGS|METH_KEYWORDS,"do_alias_get_time"},
	{"get_action", (PyCFunction)Protocol_get_action, METH_NOARGS,"do_alias_get_action"},
	{"set_action", (PyCFunction)Protocol_set_action, METH_VARARGS|METH_KEYWORDS,"do_alias_get_action"},

 {NULL}  /* Sentinel */
};

static PyMethodDef ProtocolKey0_methods[] = {
	{"get_objectName", (PyCFunction)ProtocolKey0_get_objectName, METH_NOARGS,"do_alias_get_objectName"},
	{"set_objectName", (PyCFunction)ProtocolKey0_set_objectName, METH_VARARGS|METH_KEYWORDS,"do_alias_get_objectName"},
	{"get_date", (PyCFunction)ProtocolKey0_get_date, METH_NOARGS,"do_alias_get_date"},
	{"set_date", (PyCFunction)ProtocolKey0_set_date, METH_VARARGS|METH_KEYWORDS,"do_alias_get_date"},
	{"get_time", (PyCFunction)ProtocolKey0_get_time, METH_NOARGS,"do_alias_get_time"},
	{"set_time", (PyCFunction)ProtocolKey0_set_time, METH_VARARGS|METH_KEYWORDS,"do_alias_get_time"},

 {NULL}  /* Sentinel */
};

static PyMethodDef ProtocolKey1_methods[] = {
	{"get_date", (PyCFunction)ProtocolKey1_get_date, METH_NOARGS,"do_alias_get_date"},
	{"set_date", (PyCFunction)ProtocolKey1_set_date, METH_VARARGS|METH_KEYWORDS,"do_alias_get_date"},
	{"get_time", (PyCFunction)ProtocolKey1_get_time, METH_NOARGS,"do_alias_get_time"},
	{"set_time", (PyCFunction)ProtocolKey1_set_time, METH_VARARGS|METH_KEYWORDS,"do_alias_get_time"},

 {NULL}  /* Sentinel */
};

static PyMethodDef ProtocolKey2_methods[] = {
	{"get_userName", (PyCFunction)ProtocolKey2_get_userName, METH_NOARGS,"do_alias_get_userName"},
	{"set_userName", (PyCFunction)ProtocolKey2_set_userName, METH_VARARGS|METH_KEYWORDS,"do_alias_get_userName"},
	{"get_date", (PyCFunction)ProtocolKey2_get_date, METH_NOARGS,"do_alias_get_date"},
	{"set_date", (PyCFunction)ProtocolKey2_set_date, METH_VARARGS|METH_KEYWORDS,"do_alias_get_date"},
	{"get_time", (PyCFunction)ProtocolKey2_get_time, METH_NOARGS,"do_alias_get_time"},
	{"set_time", (PyCFunction)ProtocolKey2_set_time, METH_VARARGS|METH_KEYWORDS,"do_alias_get_time"},

 {NULL}  /* Sentinel */
};

static PyMethodDef User_methods[] = {
	{"get_type", (PyCFunction)User_get_type, METH_NOARGS,"do_alias_get_type"},
	{"set_type", (PyCFunction)User_set_type, METH_VARARGS|METH_KEYWORDS,"do_alias_get_type"},
	{"get_name", (PyCFunction)User_get_name, METH_NOARGS,"do_alias_get_name"},
	{"set_name", (PyCFunction)User_set_name, METH_VARARGS|METH_KEYWORDS,"do_alias_get_name"},
	{"get_pwd", (PyCFunction)User_get_pwd, METH_NOARGS,"do_alias_get_pwd"},
	{"set_pwd", (PyCFunction)User_set_pwd, METH_VARARGS|METH_KEYWORDS,"do_alias_get_pwd"},
	{"get_supervisore", (PyCFunction)User_get_supervisore, METH_NOARGS,"do_alias_get_supervisore"},
	{"set_supervisore", (PyCFunction)User_set_supervisore, METH_VARARGS|METH_KEYWORDS,"do_alias_get_supervisore"},
	{"get_userini", (PyCFunction)User_get_userini, METH_NOARGS,"do_alias_get_userini"},
	{"set_userini", (PyCFunction)User_set_userini, METH_VARARGS|METH_KEYWORDS,"do_alias_get_userini"},
	{"get_groups", (PyCFunction)User_get_groups, METH_NOARGS,"do_alias_get_groups"},
	{"set_groups", (PyCFunction)User_set_groups, METH_VARARGS|METH_KEYWORDS,"do_alias_get_groups"},
	{"get_params", (PyCFunction)User_get_params, METH_NOARGS,"do_alias_get_params"},
	{"set_params", (PyCFunction)User_set_params, METH_VARARGS|METH_KEYWORDS,"do_alias_get_params"},

 {NULL}  /* Sentinel */
};

static PyMethodDef UserKey0_methods[] = {
	{"get_type", (PyCFunction)UserKey0_get_type, METH_NOARGS,"do_alias_get_type"},
	{"set_type", (PyCFunction)UserKey0_set_type, METH_VARARGS|METH_KEYWORDS,"do_alias_get_type"},
	{"get_name", (PyCFunction)UserKey0_get_name, METH_NOARGS,"do_alias_get_name"},
	{"set_name", (PyCFunction)UserKey0_set_name, METH_VARARGS|METH_KEYWORDS,"do_alias_get_name"},

 {NULL}  /* Sentinel */
};

static PyMethodDef ShiftKey0_methods[] = {
	{"get_cash", (PyCFunction)ShiftKey0_get_cash, METH_NOARGS,"do_alias_get_cash"},
	{"set_cash", (PyCFunction)ShiftKey0_set_cash, METH_VARARGS|METH_KEYWORDS,"do_alias_get_cash"},
	{"get_shift", (PyCFunction)ShiftKey0_get_shift, METH_NOARGS,"do_alias_get_shift"},
	{"set_shift", (PyCFunction)ShiftKey0_set_shift, METH_VARARGS|METH_KEYWORDS,"do_alias_get_shift"},

 {NULL}  /* Sentinel */
};

static PyMethodDef ShiftKey1_methods[] = {
	{"get_null1", (PyCFunction)ShiftKey1_get_null1, METH_NOARGS,"do_alias_get_null1"},
	{"set_null1", (PyCFunction)ShiftKey1_set_null1, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null1"},
	{"get_date", (PyCFunction)ShiftKey1_get_date, METH_NOARGS,"do_alias_get_date"},
	{"set_date", (PyCFunction)ShiftKey1_set_date, METH_VARARGS|METH_KEYWORDS,"do_alias_get_date"},
	{"get_null2", (PyCFunction)ShiftKey1_get_null2, METH_NOARGS,"do_alias_get_null2"},
	{"set_null2", (PyCFunction)ShiftKey1_set_null2, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null2"},
	{"get_time", (PyCFunction)ShiftKey1_get_time, METH_NOARGS,"do_alias_get_time"},
	{"set_time", (PyCFunction)ShiftKey1_set_time, METH_VARARGS|METH_KEYWORDS,"do_alias_get_time"},

 {NULL}  /* Sentinel */
};

static PyMethodDef ShiftKey2_methods[] = {
	{"get_null1", (PyCFunction)ShiftKey2_get_null1, METH_NOARGS,"do_alias_get_null1"},
	{"set_null1", (PyCFunction)ShiftKey2_set_null1, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null1"},
	{"get_date_b", (PyCFunction)ShiftKey2_get_date_b, METH_NOARGS,"do_alias_get_date_b"},
	{"set_date_b", (PyCFunction)ShiftKey2_set_date_b, METH_VARARGS|METH_KEYWORDS,"do_alias_get_date_b"},
	{"get_null2", (PyCFunction)ShiftKey2_get_null2, METH_NOARGS,"do_alias_get_null2"},
	{"set_null2", (PyCFunction)ShiftKey2_set_null2, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null2"},
	{"get_time_b", (PyCFunction)ShiftKey2_get_time_b, METH_NOARGS,"do_alias_get_time_b"},
	{"set_time_b", (PyCFunction)ShiftKey2_set_time_b, METH_VARARGS|METH_KEYWORDS,"do_alias_get_time_b"},

 {NULL}  /* Sentinel */
};

static PyMethodDef ShiftKey3_methods[] = {
	{"get_null1", (PyCFunction)ShiftKey3_get_null1, METH_NOARGS,"do_alias_get_null1"},
	{"set_null1", (PyCFunction)ShiftKey3_set_null1, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null1"},
	{"get_sklad", (PyCFunction)ShiftKey3_get_sklad, METH_NOARGS,"do_alias_get_sklad"},
	{"set_sklad", (PyCFunction)ShiftKey3_set_sklad, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sklad"},
	{"get_null2", (PyCFunction)ShiftKey3_get_null2, METH_NOARGS,"do_alias_get_null2"},
	{"set_null2", (PyCFunction)ShiftKey3_set_null2, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null2"},
	{"get_date", (PyCFunction)ShiftKey3_get_date, METH_NOARGS,"do_alias_get_date"},
	{"set_date", (PyCFunction)ShiftKey3_set_date, METH_VARARGS|METH_KEYWORDS,"do_alias_get_date"},
	{"get_null3", (PyCFunction)ShiftKey3_get_null3, METH_NOARGS,"do_alias_get_null3"},
	{"set_null3", (PyCFunction)ShiftKey3_set_null3, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null3"},
	{"get_time", (PyCFunction)ShiftKey3_get_time, METH_NOARGS,"do_alias_get_time"},
	{"set_time", (PyCFunction)ShiftKey3_set_time, METH_VARARGS|METH_KEYWORDS,"do_alias_get_time"},

 {NULL}  /* Sentinel */
};

static PyMethodDef Checksum_methods[] = {
	{"get_null1", (PyCFunction)Checksum_get_null1, METH_NOARGS,"do_alias_get_null1"},
	{"set_null1", (PyCFunction)Checksum_set_null1, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null1"},
	{"get_cash", (PyCFunction)Checksum_get_cash, METH_NOARGS,"do_alias_get_cash"},
	{"set_cash", (PyCFunction)Checksum_set_cash, METH_VARARGS|METH_KEYWORDS,"do_alias_get_cash"},
	{"get_null2", (PyCFunction)Checksum_get_null2, METH_NOARGS,"do_alias_get_null2"},
	{"set_null2", (PyCFunction)Checksum_set_null2, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null2"},
	{"get_shift", (PyCFunction)Checksum_get_shift, METH_NOARGS,"do_alias_get_shift"},
	{"set_shift", (PyCFunction)Checksum_set_shift, METH_VARARGS|METH_KEYWORDS,"do_alias_get_shift"},
	{"get_null3", (PyCFunction)Checksum_get_null3, METH_NOARGS,"do_alias_get_null3"},
	{"set_null3", (PyCFunction)Checksum_set_null3, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null3"},
	{"get_check", (PyCFunction)Checksum_get_check, METH_NOARGS,"do_alias_get_check"},
	{"set_check", (PyCFunction)Checksum_set_check, METH_VARARGS|METH_KEYWORDS,"do_alias_get_check"},
	{"get_null4", (PyCFunction)Checksum_get_null4, METH_NOARGS,"do_alias_get_null4"},
	{"set_null4", (PyCFunction)Checksum_set_null4, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null4"},
	{"get_date", (PyCFunction)Checksum_get_date, METH_NOARGS,"do_alias_get_date"},
	{"set_date", (PyCFunction)Checksum_set_date, METH_VARARGS|METH_KEYWORDS,"do_alias_get_date"},
	{"get_null5", (PyCFunction)Checksum_get_null5, METH_NOARGS,"do_alias_get_null5"},
	{"set_null5", (PyCFunction)Checksum_set_null5, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null5"},
	{"get_time", (PyCFunction)Checksum_get_time, METH_NOARGS,"do_alias_get_time"},
	{"set_time", (PyCFunction)Checksum_set_time, METH_VARARGS|METH_KEYWORDS,"do_alias_get_time"},
	{"get_null6", (PyCFunction)Checksum_get_null6, METH_NOARGS,"do_alias_get_null6"},
	{"set_null6", (PyCFunction)Checksum_set_null6, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null6"},
	{"get_cashier", (PyCFunction)Checksum_get_cashier, METH_NOARGS,"do_alias_get_cashier"},
	{"set_cashier", (PyCFunction)Checksum_set_cashier, METH_VARARGS|METH_KEYWORDS,"do_alias_get_cashier"},
	{"get_null7", (PyCFunction)Checksum_get_null7, METH_NOARGS,"do_alias_get_null7"},
	{"set_null7", (PyCFunction)Checksum_set_null7, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null7"},
	{"get_valuta", (PyCFunction)Checksum_get_valuta, METH_NOARGS,"do_alias_get_valuta"},
	{"set_valuta", (PyCFunction)Checksum_set_valuta, METH_VARARGS|METH_KEYWORDS,"do_alias_get_valuta"},
	{"get_null8", (PyCFunction)Checksum_get_null8, METH_NOARGS,"do_alias_get_null8"},
	{"set_null8", (PyCFunction)Checksum_set_null8, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null8"},
	{"get_operation", (PyCFunction)Checksum_get_operation, METH_NOARGS,"do_alias_get_operation"},
	{"set_operation", (PyCFunction)Checksum_set_operation, METH_VARARGS|METH_KEYWORDS,"do_alias_get_operation"},
	{"get_null9", (PyCFunction)Checksum_get_null9, METH_NOARGS,"do_alias_get_null9"},
	{"set_null9", (PyCFunction)Checksum_set_null9, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null9"},
	{"get_sum", (PyCFunction)Checksum_get_sum, METH_NOARGS,"do_alias_get_sum"},
	{"set_sum", (PyCFunction)Checksum_set_sum, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sum"},
	{"get_null10", (PyCFunction)Checksum_get_null10, METH_NOARGS,"do_alias_get_null10"},
	{"set_null10", (PyCFunction)Checksum_set_null10, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null10"},
	{"get_total", (PyCFunction)Checksum_get_total, METH_NOARGS,"do_alias_get_total"},
	{"set_total", (PyCFunction)Checksum_set_total, METH_VARARGS|METH_KEYWORDS,"do_alias_get_total"},

 {NULL}  /* Sentinel */
};

static PyMethodDef ChecksumKey0_methods[] = {
	{"get_null1", (PyCFunction)ChecksumKey0_get_null1, METH_NOARGS,"do_alias_get_null1"},
	{"set_null1", (PyCFunction)ChecksumKey0_set_null1, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null1"},
	{"get_cash", (PyCFunction)ChecksumKey0_get_cash, METH_NOARGS,"do_alias_get_cash"},
	{"set_cash", (PyCFunction)ChecksumKey0_set_cash, METH_VARARGS|METH_KEYWORDS,"do_alias_get_cash"},
	{"get_null2", (PyCFunction)ChecksumKey0_get_null2, METH_NOARGS,"do_alias_get_null2"},
	{"set_null2", (PyCFunction)ChecksumKey0_set_null2, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null2"},
	{"get_shift", (PyCFunction)ChecksumKey0_get_shift, METH_NOARGS,"do_alias_get_shift"},
	{"set_shift", (PyCFunction)ChecksumKey0_set_shift, METH_VARARGS|METH_KEYWORDS,"do_alias_get_shift"},
	{"get_null4", (PyCFunction)ChecksumKey0_get_null4, METH_NOARGS,"do_alias_get_null4"},
	{"set_null4", (PyCFunction)ChecksumKey0_set_null4, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null4"},
	{"get_date", (PyCFunction)ChecksumKey0_get_date, METH_NOARGS,"do_alias_get_date"},
	{"set_date", (PyCFunction)ChecksumKey0_set_date, METH_VARARGS|METH_KEYWORDS,"do_alias_get_date"},
	{"get_null5", (PyCFunction)ChecksumKey0_get_null5, METH_NOARGS,"do_alias_get_null5"},
	{"set_null5", (PyCFunction)ChecksumKey0_set_null5, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null5"},
	{"get_time", (PyCFunction)ChecksumKey0_get_time, METH_NOARGS,"do_alias_get_time"},
	{"set_time", (PyCFunction)ChecksumKey0_set_time, METH_VARARGS|METH_KEYWORDS,"do_alias_get_time"},
	{"get_null3", (PyCFunction)ChecksumKey0_get_null3, METH_NOARGS,"do_alias_get_null3"},
	{"set_null3", (PyCFunction)ChecksumKey0_set_null3, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null3"},
	{"get_check", (PyCFunction)ChecksumKey0_get_check, METH_NOARGS,"do_alias_get_check"},
	{"set_check", (PyCFunction)ChecksumKey0_set_check, METH_VARARGS|METH_KEYWORDS,"do_alias_get_check"},

 {NULL}  /* Sentinel */
};

static PyMethodDef ChecksumKey1_methods[] = {
	{"get_null4", (PyCFunction)ChecksumKey1_get_null4, METH_NOARGS,"do_alias_get_null4"},
	{"set_null4", (PyCFunction)ChecksumKey1_set_null4, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null4"},
	{"get_date", (PyCFunction)ChecksumKey1_get_date, METH_NOARGS,"do_alias_get_date"},
	{"set_date", (PyCFunction)ChecksumKey1_set_date, METH_VARARGS|METH_KEYWORDS,"do_alias_get_date"},
	{"get_null5", (PyCFunction)ChecksumKey1_get_null5, METH_NOARGS,"do_alias_get_null5"},
	{"set_null5", (PyCFunction)ChecksumKey1_set_null5, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null5"},
	{"get_time", (PyCFunction)ChecksumKey1_get_time, METH_NOARGS,"do_alias_get_time"},
	{"set_time", (PyCFunction)ChecksumKey1_set_time, METH_VARARGS|METH_KEYWORDS,"do_alias_get_time"},
	{"get_null1", (PyCFunction)ChecksumKey1_get_null1, METH_NOARGS,"do_alias_get_null1"},
	{"set_null1", (PyCFunction)ChecksumKey1_set_null1, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null1"},
	{"get_cash", (PyCFunction)ChecksumKey1_get_cash, METH_NOARGS,"do_alias_get_cash"},
	{"set_cash", (PyCFunction)ChecksumKey1_set_cash, METH_VARARGS|METH_KEYWORDS,"do_alias_get_cash"},

 {NULL}  /* Sentinel */
};

static PyMethodDef Check_methods[] = {
	{"get_cash", (PyCFunction)Check_get_cash, METH_NOARGS,"do_alias_get_cash"},
	{"set_cash", (PyCFunction)Check_set_cash, METH_VARARGS|METH_KEYWORDS,"do_alias_get_cash"},
	{"get_null1", (PyCFunction)Check_get_null1, METH_NOARGS,"do_alias_get_null1"},
	{"set_null1", (PyCFunction)Check_set_null1, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null1"},
	{"get_shift", (PyCFunction)Check_get_shift, METH_NOARGS,"do_alias_get_shift"},
	{"set_shift", (PyCFunction)Check_set_shift, METH_VARARGS|METH_KEYWORDS,"do_alias_get_shift"},
	{"get_null2", (PyCFunction)Check_get_null2, METH_NOARGS,"do_alias_get_null2"},
	{"set_null2", (PyCFunction)Check_set_null2, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null2"},
	{"get_check", (PyCFunction)Check_get_check, METH_NOARGS,"do_alias_get_check"},
	{"set_check", (PyCFunction)Check_set_check, METH_VARARGS|METH_KEYWORDS,"do_alias_get_check"},
	{"get_null3", (PyCFunction)Check_get_null3, METH_NOARGS,"do_alias_get_null3"},
	{"set_null3", (PyCFunction)Check_set_null3, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null3"},
	{"get_position", (PyCFunction)Check_get_position, METH_NOARGS,"do_alias_get_position"},
	{"set_position", (PyCFunction)Check_set_position, METH_VARARGS|METH_KEYWORDS,"do_alias_get_position"},
	{"get_null4", (PyCFunction)Check_get_null4, METH_NOARGS,"do_alias_get_null4"},
	{"set_null4", (PyCFunction)Check_set_null4, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null4"},
	{"get_date", (PyCFunction)Check_get_date, METH_NOARGS,"do_alias_get_date"},
	{"set_date", (PyCFunction)Check_set_date, METH_VARARGS|METH_KEYWORDS,"do_alias_get_date"},
	{"get_null5", (PyCFunction)Check_get_null5, METH_NOARGS,"do_alias_get_null5"},
	{"set_null5", (PyCFunction)Check_set_null5, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null5"},
	{"get_time", (PyCFunction)Check_get_time, METH_NOARGS,"do_alias_get_time"},
	{"set_time", (PyCFunction)Check_set_time, METH_VARARGS|METH_KEYWORDS,"do_alias_get_time"},
	{"get_null6", (PyCFunction)Check_get_null6, METH_NOARGS,"do_alias_get_null6"},
	{"set_null6", (PyCFunction)Check_set_null6, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null6"},
	{"get_price", (PyCFunction)Check_get_price, METH_NOARGS,"do_alias_get_price"},
	{"set_price", (PyCFunction)Check_set_price, METH_VARARGS|METH_KEYWORDS,"do_alias_get_price"},
	{"get_null7", (PyCFunction)Check_get_null7, METH_NOARGS,"do_alias_get_null7"},
	{"set_null7", (PyCFunction)Check_set_null7, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null7"},
	{"get_quantity", (PyCFunction)Check_get_quantity, METH_NOARGS,"do_alias_get_quantity"},
	{"set_quantity", (PyCFunction)Check_set_quantity, METH_VARARGS|METH_KEYWORDS,"do_alias_get_quantity"},
	{"get_null8", (PyCFunction)Check_get_null8, METH_NOARGS,"do_alias_get_null8"},
	{"set_null8", (PyCFunction)Check_set_null8, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null8"},
	{"get_product_code", (PyCFunction)Check_get_product_code, METH_NOARGS,"do_alias_get_product_code"},
	{"set_product_code", (PyCFunction)Check_set_product_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_product_code"},
	{"get_null9", (PyCFunction)Check_get_null9, METH_NOARGS,"do_alias_get_null9"},
	{"set_null9", (PyCFunction)Check_set_null9, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null9"},
	{"get_bar_code", (PyCFunction)Check_get_bar_code, METH_NOARGS,"do_alias_get_bar_code"},
	{"set_bar_code", (PyCFunction)Check_set_bar_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_bar_code"},
	{"get_null10", (PyCFunction)Check_get_null10, METH_NOARGS,"do_alias_get_null10"},
	{"set_null10", (PyCFunction)Check_set_null10, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null10"},
	{"get_cashier", (PyCFunction)Check_get_cashier, METH_NOARGS,"do_alias_get_cashier"},
	{"set_cashier", (PyCFunction)Check_set_cashier, METH_VARARGS|METH_KEYWORDS,"do_alias_get_cashier"},
	{"get_null11", (PyCFunction)Check_get_null11, METH_NOARGS,"do_alias_get_null11"},
	{"set_null11", (PyCFunction)Check_set_null11, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null11"},
	{"get_valuta", (PyCFunction)Check_get_valuta, METH_NOARGS,"do_alias_get_valuta"},
	{"set_valuta", (PyCFunction)Check_set_valuta, METH_VARARGS|METH_KEYWORDS,"do_alias_get_valuta"},
	{"get_null12", (PyCFunction)Check_get_null12, METH_NOARGS,"do_alias_get_null12"},
	{"set_null12", (PyCFunction)Check_set_null12, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null12"},
	{"get_operation", (PyCFunction)Check_get_operation, METH_NOARGS,"do_alias_get_operation"},
	{"set_operation", (PyCFunction)Check_set_operation, METH_VARARGS|METH_KEYWORDS,"do_alias_get_operation"},

 {NULL}  /* Sentinel */
};

static PyMethodDef CheckKey0_methods[] = {
	{"get_cash", (PyCFunction)CheckKey0_get_cash, METH_NOARGS,"do_alias_get_cash"},
	{"set_cash", (PyCFunction)CheckKey0_set_cash, METH_VARARGS|METH_KEYWORDS,"do_alias_get_cash"},
	{"get_null1", (PyCFunction)CheckKey0_get_null1, METH_NOARGS,"do_alias_get_null1"},
	{"set_null1", (PyCFunction)CheckKey0_set_null1, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null1"},
	{"get_shift", (PyCFunction)CheckKey0_get_shift, METH_NOARGS,"do_alias_get_shift"},
	{"set_shift", (PyCFunction)CheckKey0_set_shift, METH_VARARGS|METH_KEYWORDS,"do_alias_get_shift"},
	{"get_null2", (PyCFunction)CheckKey0_get_null2, METH_NOARGS,"do_alias_get_null2"},
	{"set_null2", (PyCFunction)CheckKey0_set_null2, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null2"},
	{"get_check", (PyCFunction)CheckKey0_get_check, METH_NOARGS,"do_alias_get_check"},
	{"set_check", (PyCFunction)CheckKey0_set_check, METH_VARARGS|METH_KEYWORDS,"do_alias_get_check"},
	{"get_null3", (PyCFunction)CheckKey0_get_null3, METH_NOARGS,"do_alias_get_null3"},
	{"set_null3", (PyCFunction)CheckKey0_set_null3, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null3"},
	{"get_position", (PyCFunction)CheckKey0_get_position, METH_NOARGS,"do_alias_get_position"},
	{"set_position", (PyCFunction)CheckKey0_set_position, METH_VARARGS|METH_KEYWORDS,"do_alias_get_position"},
	{"get_null4", (PyCFunction)CheckKey0_get_null4, METH_NOARGS,"do_alias_get_null4"},
	{"set_null4", (PyCFunction)CheckKey0_set_null4, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null4"},
	{"get_date", (PyCFunction)CheckKey0_get_date, METH_NOARGS,"do_alias_get_date"},
	{"set_date", (PyCFunction)CheckKey0_set_date, METH_VARARGS|METH_KEYWORDS,"do_alias_get_date"},
	{"get_null5", (PyCFunction)CheckKey0_get_null5, METH_NOARGS,"do_alias_get_null5"},
	{"set_null5", (PyCFunction)CheckKey0_set_null5, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null5"},
	{"get_time", (PyCFunction)CheckKey0_get_time, METH_NOARGS,"do_alias_get_time"},
	{"set_time", (PyCFunction)CheckKey0_set_time, METH_VARARGS|METH_KEYWORDS,"do_alias_get_time"},

 {NULL}  /* Sentinel */
};

static PyMethodDef CheckKey1_methods[] = {
	{"get_null1", (PyCFunction)CheckKey1_get_null1, METH_NOARGS,"do_alias_get_null1"},
	{"set_null1", (PyCFunction)CheckKey1_set_null1, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null1"},
	{"get_date", (PyCFunction)CheckKey1_get_date, METH_NOARGS,"do_alias_get_date"},
	{"set_date", (PyCFunction)CheckKey1_set_date, METH_VARARGS|METH_KEYWORDS,"do_alias_get_date"},
	{"get_null2", (PyCFunction)CheckKey1_get_null2, METH_NOARGS,"do_alias_get_null2"},
	{"set_null2", (PyCFunction)CheckKey1_set_null2, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null2"},
	{"get_time", (PyCFunction)CheckKey1_get_time, METH_NOARGS,"do_alias_get_time"},
	{"set_time", (PyCFunction)CheckKey1_set_time, METH_VARARGS|METH_KEYWORDS,"do_alias_get_time"},

 {NULL}  /* Sentinel */
};

static PyMethodDef CheckKey2_methods[] = {
	{"get_null1", (PyCFunction)CheckKey2_get_null1, METH_NOARGS,"do_alias_get_null1"},
	{"set_null1", (PyCFunction)CheckKey2_set_null1, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null1"},
	{"get_product_code", (PyCFunction)CheckKey2_get_product_code, METH_NOARGS,"do_alias_get_product_code"},
	{"set_product_code", (PyCFunction)CheckKey2_set_product_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_product_code"},
	{"get_null2", (PyCFunction)CheckKey2_get_null2, METH_NOARGS,"do_alias_get_null2"},
	{"set_null2", (PyCFunction)CheckKey2_set_null2, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null2"},
	{"get_date", (PyCFunction)CheckKey2_get_date, METH_NOARGS,"do_alias_get_date"},
	{"set_date", (PyCFunction)CheckKey2_set_date, METH_VARARGS|METH_KEYWORDS,"do_alias_get_date"},
	{"get_null3", (PyCFunction)CheckKey2_get_null3, METH_NOARGS,"do_alias_get_null3"},
	{"set_null3", (PyCFunction)CheckKey2_set_null3, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null3"},
	{"get_time", (PyCFunction)CheckKey2_get_time, METH_NOARGS,"do_alias_get_time"},
	{"set_time", (PyCFunction)CheckKey2_set_time, METH_VARARGS|METH_KEYWORDS,"do_alias_get_time"},
	{"get_cash", (PyCFunction)CheckKey2_get_cash, METH_NOARGS,"do_alias_get_cash"},
	{"set_cash", (PyCFunction)CheckKey2_set_cash, METH_VARARGS|METH_KEYWORDS,"do_alias_get_cash"},
	{"get_null4", (PyCFunction)CheckKey2_get_null4, METH_NOARGS,"do_alias_get_null4"},
	{"set_null4", (PyCFunction)CheckKey2_set_null4, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null4"},
	{"get_shift", (PyCFunction)CheckKey2_get_shift, METH_NOARGS,"do_alias_get_shift"},
	{"set_shift", (PyCFunction)CheckKey2_set_shift, METH_VARARGS|METH_KEYWORDS,"do_alias_get_shift"},
	{"get_null5", (PyCFunction)CheckKey2_get_null5, METH_NOARGS,"do_alias_get_null5"},
	{"set_null5", (PyCFunction)CheckKey2_set_null5, METH_VARARGS|METH_KEYWORDS,"do_alias_get_null5"},
	{"get_check", (PyCFunction)CheckKey2_get_check, METH_NOARGS,"do_alias_get_check"},
	{"set_check", (PyCFunction)CheckKey2_set_check, METH_VARARGS|METH_KEYWORDS,"do_alias_get_check"},

 {NULL}  /* Sentinel */
};

static PyMethodDef Realization_methods[] = {
	{"get_sklad", (PyCFunction)Realization_get_sklad, METH_NOARGS,"do_alias_get_sklad"},
	{"set_sklad", (PyCFunction)Realization_set_sklad, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sklad"},
	{"get_code", (PyCFunction)Realization_get_code, METH_NOARGS,"do_alias_get_code"},
	{"set_code", (PyCFunction)Realization_set_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_code"},
	{"get_month", (PyCFunction)Realization_get_month, METH_NOARGS,"do_alias_get_month"},
	{"set_month", (PyCFunction)Realization_set_month, METH_VARARGS|METH_KEYWORDS,"do_alias_get_month"},
	{"get_total", (PyCFunction)Realization_get_total, METH_NOARGS,"do_alias_get_total"},
	{"set_total", (PyCFunction)Realization_set_total, METH_VARARGS|METH_KEYWORDS,"do_alias_get_total"},
	{"get_sum", (PyCFunction)Realization_get_sum, METH_NOARGS,"do_alias_get_sum"},
	{"set_sum", (PyCFunction)Realization_set_sum, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sum"},
	{"get_profit", (PyCFunction)Realization_get_profit, METH_NOARGS,"do_alias_get_profit"},
	{"set_profit", (PyCFunction)Realization_set_profit, METH_VARARGS|METH_KEYWORDS,"do_alias_get_profit"},

 {NULL}  /* Sentinel */
};

static PyMethodDef RealizationKey0_methods[] = {
	{"get_code", (PyCFunction)RealizationKey0_get_code, METH_NOARGS,"do_alias_get_code"},
	{"set_code", (PyCFunction)RealizationKey0_set_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_code"},
	{"get_month", (PyCFunction)RealizationKey0_get_month, METH_NOARGS,"do_alias_get_month"},
	{"set_month", (PyCFunction)RealizationKey0_set_month, METH_VARARGS|METH_KEYWORDS,"do_alias_get_month"},
	{"get_sklad", (PyCFunction)RealizationKey0_get_sklad, METH_NOARGS,"do_alias_get_sklad"},
	{"set_sklad", (PyCFunction)RealizationKey0_set_sklad, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sklad"},

 {NULL}  /* Sentinel */
};

static PyMethodDef RealizationKey1_methods[] = {
	{"get_sklad", (PyCFunction)RealizationKey1_get_sklad, METH_NOARGS,"do_alias_get_sklad"},
	{"set_sklad", (PyCFunction)RealizationKey1_set_sklad, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sklad"},
	{"get_month", (PyCFunction)RealizationKey1_get_month, METH_NOARGS,"do_alias_get_month"},
	{"set_month", (PyCFunction)RealizationKey1_set_month, METH_VARARGS|METH_KEYWORDS,"do_alias_get_month"},
	{"get_code", (PyCFunction)RealizationKey1_get_code, METH_NOARGS,"do_alias_get_code"},
	{"set_code", (PyCFunction)RealizationKey1_set_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_code"},

 {NULL}  /* Sentinel */
};

static PyMethodDef RealizationKey2_methods[] = {
	{"get_month", (PyCFunction)RealizationKey2_get_month, METH_NOARGS,"do_alias_get_month"},
	{"set_month", (PyCFunction)RealizationKey2_set_month, METH_VARARGS|METH_KEYWORDS,"do_alias_get_month"},
	{"get_code", (PyCFunction)RealizationKey2_get_code, METH_NOARGS,"do_alias_get_code"},
	{"set_code", (PyCFunction)RealizationKey2_set_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_code"},
	{"get_sklad", (PyCFunction)RealizationKey2_get_sklad, METH_NOARGS,"do_alias_get_sklad"},
	{"set_sklad", (PyCFunction)RealizationKey2_set_sklad, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sklad"},

 {NULL}  /* Sentinel */
};

static PyMethodDef RealizationKey3_methods[] = {
	{"get_month", (PyCFunction)RealizationKey3_get_month, METH_NOARGS,"do_alias_get_month"},
	{"set_month", (PyCFunction)RealizationKey3_set_month, METH_VARARGS|METH_KEYWORDS,"do_alias_get_month"},
	{"get_sklad", (PyCFunction)RealizationKey3_get_sklad, METH_NOARGS,"do_alias_get_sklad"},
	{"set_sklad", (PyCFunction)RealizationKey3_set_sklad, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sklad"},
	{"get_code", (PyCFunction)RealizationKey3_get_code, METH_NOARGS,"do_alias_get_code"},
	{"set_code", (PyCFunction)RealizationKey3_set_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_code"},

 {NULL}  /* Sentinel */
};

static PyMethodDef RealizationKey4_methods[] = {
	{"get_code", (PyCFunction)RealizationKey4_get_code, METH_NOARGS,"do_alias_get_code"},
	{"set_code", (PyCFunction)RealizationKey4_set_code, METH_VARARGS|METH_KEYWORDS,"do_alias_get_code"},
	{"get_sklad", (PyCFunction)RealizationKey4_get_sklad, METH_NOARGS,"do_alias_get_sklad"},
	{"set_sklad", (PyCFunction)RealizationKey4_set_sklad, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sklad"},
	{"get_month", (PyCFunction)RealizationKey4_get_month, METH_NOARGS,"do_alias_get_month"},
	{"set_month", (PyCFunction)RealizationKey4_set_month, METH_VARARGS|METH_KEYWORDS,"do_alias_get_month"},

 {NULL}  /* Sentinel */
};

static PyMethodDef Discount_methods[] = {
	{"get_card", (PyCFunction)Discount_get_card, METH_NOARGS,"do_alias_get_card"},
	{"set_card", (PyCFunction)Discount_set_card, METH_VARARGS|METH_KEYWORDS,"do_alias_get_card"},
	{"get_cash", (PyCFunction)Discount_get_cash, METH_NOARGS,"do_alias_get_cash"},
	{"set_cash", (PyCFunction)Discount_set_cash, METH_VARARGS|METH_KEYWORDS,"do_alias_get_cash"},
	{"get_shift", (PyCFunction)Discount_get_shift, METH_NOARGS,"do_alias_get_shift"},
	{"set_shift", (PyCFunction)Discount_set_shift, METH_VARARGS|METH_KEYWORDS,"do_alias_get_shift"},
	{"get_check", (PyCFunction)Discount_get_check, METH_NOARGS,"do_alias_get_check"},
	{"set_check", (PyCFunction)Discount_set_check, METH_VARARGS|METH_KEYWORDS,"do_alias_get_check"},
	{"get_date", (PyCFunction)Discount_get_date, METH_NOARGS,"do_alias_get_date"},
	{"set_date", (PyCFunction)Discount_set_date, METH_VARARGS|METH_KEYWORDS,"do_alias_get_date"},
	{"get_time", (PyCFunction)Discount_get_time, METH_NOARGS,"do_alias_get_time"},
	{"set_time", (PyCFunction)Discount_set_time, METH_VARARGS|METH_KEYWORDS,"do_alias_get_time"},
	{"get_sum", (PyCFunction)Discount_get_sum, METH_NOARGS,"do_alias_get_sum"},
	{"set_sum", (PyCFunction)Discount_set_sum, METH_VARARGS|METH_KEYWORDS,"do_alias_get_sum"},
	{"get_discount", (PyCFunction)Discount_get_discount, METH_NOARGS,"do_alias_get_discount"},
	{"set_discount", (PyCFunction)Discount_set_discount, METH_VARARGS|METH_KEYWORDS,"do_alias_get_discount"},

 {NULL}  /* Sentinel */
};

static PyMethodDef DiscountKey0_methods[] = {
	{"get_cash", (PyCFunction)DiscountKey0_get_cash, METH_NOARGS,"do_alias_get_cash"},
	{"set_cash", (PyCFunction)DiscountKey0_set_cash, METH_VARARGS|METH_KEYWORDS,"do_alias_get_cash"},
	{"get_shift", (PyCFunction)DiscountKey0_get_shift, METH_NOARGS,"do_alias_get_shift"},
	{"set_shift", (PyCFunction)DiscountKey0_set_shift, METH_VARARGS|METH_KEYWORDS,"do_alias_get_shift"},
	{"get_date", (PyCFunction)DiscountKey0_get_date, METH_NOARGS,"do_alias_get_date"},
	{"set_date", (PyCFunction)DiscountKey0_set_date, METH_VARARGS|METH_KEYWORDS,"do_alias_get_date"},
	{"get_check", (PyCFunction)DiscountKey0_get_check, METH_NOARGS,"do_alias_get_check"},
	{"set_check", (PyCFunction)DiscountKey0_set_check, METH_VARARGS|METH_KEYWORDS,"do_alias_get_check"},
	{"get_time", (PyCFunction)DiscountKey0_get_time, METH_NOARGS,"do_alias_get_time"},
	{"set_time", (PyCFunction)DiscountKey0_set_time, METH_VARARGS|METH_KEYWORDS,"do_alias_get_time"},

 {NULL}  /* Sentinel */
};

static PyMethodDef DiscountKey1_methods[] = {
	{"get_card", (PyCFunction)DiscountKey1_get_card, METH_NOARGS,"do_alias_get_card"},
	{"set_card", (PyCFunction)DiscountKey1_set_card, METH_VARARGS|METH_KEYWORDS,"do_alias_get_card"},
	{"get_date", (PyCFunction)DiscountKey1_get_date, METH_NOARGS,"do_alias_get_date"},
	{"set_date", (PyCFunction)DiscountKey1_set_date, METH_VARARGS|METH_KEYWORDS,"do_alias_get_date"},
	{"get_time", (PyCFunction)DiscountKey1_get_time, METH_NOARGS,"do_alias_get_time"},
	{"set_time", (PyCFunction)DiscountKey1_set_time, METH_VARARGS|METH_KEYWORDS,"do_alias_get_time"},

 {NULL}  /* Sentinel */
};

static PyMethodDef StructFile_methods[] = {
	{"get_id", (PyCFunction)StructFile_get_id, METH_NOARGS,"do_alias_get_id"},
	{"set_id", (PyCFunction)StructFile_set_id, METH_VARARGS|METH_KEYWORDS,"do_alias_get_id"},
	{"get_name", (PyCFunction)StructFile_get_name, METH_NOARGS,"do_alias_get_name"},
	{"set_name", (PyCFunction)StructFile_set_name, METH_VARARGS|METH_KEYWORDS,"do_alias_get_name"},
	{"get_location", (PyCFunction)StructFile_get_location, METH_NOARGS,"do_alias_get_location"},
	{"set_location", (PyCFunction)StructFile_set_location, METH_VARARGS|METH_KEYWORDS,"do_alias_get_location"},
	{"get_other", (PyCFunction)StructFile_get_other, METH_NOARGS,"do_alias_get_other"},
	{"set_other", (PyCFunction)StructFile_set_other, METH_VARARGS|METH_KEYWORDS,"do_alias_get_other"},

 {NULL}  /* Sentinel */
};

static PyMethodDef StructFileKey0_methods[] = {
	{"get_id", (PyCFunction)StructFileKey0_get_id, METH_NOARGS,"do_alias_get_id"},
	{"set_id", (PyCFunction)StructFileKey0_set_id, METH_VARARGS|METH_KEYWORDS,"do_alias_get_id"},

 {NULL}  /* Sentinel */
};

static PyMethodDef StructFileKey1_methods[] = {
	{"get_name", (PyCFunction)StructFileKey1_get_name, METH_NOARGS,"do_alias_get_name"},
	{"set_name", (PyCFunction)StructFileKey1_set_name, METH_VARARGS|METH_KEYWORDS,"do_alias_get_name"},

 {NULL}  /* Sentinel */
};

//!!!!!!

static PyTypeObject DocumentKey0Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.DocumentKey0",             /*tp_name*/
    sizeof(DocumentKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentKey0_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentKey0_new,                 /* tp_new */
};

static PyTypeObject DocumentKey1Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.DocumentKey1",             /*tp_name*/
    sizeof(DocumentKey1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentKey1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentKey1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentKey1_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentKey1_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentKey1_new,                 /* tp_new */
};

static PyTypeObject DocumentKey2Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.DocumentKey2",             /*tp_name*/
    sizeof(DocumentKey2),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentKey2_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentKey2 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentKey2_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentKey2_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentKey2_new,                 /* tp_new */
};

static PyTypeObject DocumentKey3Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.DocumentKey3",             /*tp_name*/
    sizeof(DocumentKey3),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentKey3_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentKey3 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentKey3_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentKey3_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentKey3_new,                 /* tp_new */
};

static PyTypeObject DocumentType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.Document",             /*tp_name*/
    sizeof(Document),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Document_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Document objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Document_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Document_init,      /* tp_init */
    0,                         /* tp_alloc */
    Document_new,                 /* tp_new */
};

static PyTypeObject DocumentOrderKey0Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.DocumentOrderKey0",             /*tp_name*/
    sizeof(DocumentOrderKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentOrderKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentOrderKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentOrderKey0_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentOrderKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentOrderKey0_new,                 /* tp_new */
};

static PyTypeObject DocumentOrderKey1Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.DocumentOrderKey1",             /*tp_name*/
    sizeof(DocumentOrderKey1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentOrderKey1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentOrderKey1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentOrderKey1_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentOrderKey1_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentOrderKey1_new,                 /* tp_new */
};

static PyTypeObject DocumentOrderKey2Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.DocumentOrderKey2",             /*tp_name*/
    sizeof(DocumentOrderKey2),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentOrderKey2_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentOrderKey2 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentOrderKey2_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentOrderKey2_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentOrderKey2_new,                 /* tp_new */
};

static PyTypeObject DocumentOrderKey3Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.DocumentOrderKey3",             /*tp_name*/
    sizeof(DocumentOrderKey3),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentOrderKey3_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentOrderKey3 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentOrderKey3_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentOrderKey3_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentOrderKey3_new,                 /* tp_new */
};

static PyTypeObject DocumentOrderKey4Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.DocumentOrderKey4",             /*tp_name*/
    sizeof(DocumentOrderKey4),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentOrderKey4_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentOrderKey4 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentOrderKey4_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentOrderKey4_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentOrderKey4_new,                 /* tp_new */
};

static PyTypeObject DocumentOrderType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.DocumentOrder",             /*tp_name*/
    sizeof(DocumentOrder),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentOrder_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentOrder objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentOrder_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentOrder_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentOrder_new,                 /* tp_new */
};

static PyTypeObject DocumentLinkKey0Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.DocumentLinkKey0",             /*tp_name*/
    sizeof(DocumentLinkKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentLinkKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentLinkKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentLinkKey0_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentLinkKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentLinkKey0_new,                 /* tp_new */
};

static PyTypeObject DocumentLinkKey1Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.DocumentLinkKey1",             /*tp_name*/
    sizeof(DocumentLinkKey1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentLinkKey1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentLinkKey1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentLinkKey1_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentLinkKey1_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentLinkKey1_new,                 /* tp_new */
};

static PyTypeObject DocumentLinkType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.DocumentLink",             /*tp_name*/
    sizeof(DocumentLink),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentLink_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentLink objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentLink_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentLink_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentLink_new,                 /* tp_new */
};

static PyTypeObject EnumKey0Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.EnumKey0",             /*tp_name*/
    sizeof(EnumKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)EnumKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "EnumKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    EnumKey0_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)EnumKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    EnumKey0_new,                 /* tp_new */
};

static PyTypeObject EnumType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.Enum",             /*tp_name*/
    sizeof(Enum),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Enum_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Enum objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Enum_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Enum_init,      /* tp_init */
    0,                         /* tp_alloc */
    Enum_new,                 /* tp_new */
};

static PyTypeObject DocumentViewKey0Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.DocumentViewKey0",             /*tp_name*/
    sizeof(DocumentViewKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentViewKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentViewKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentViewKey0_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentViewKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentViewKey0_new,                 /* tp_new */
};

static PyTypeObject DocumentViewKey1Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.DocumentViewKey1",             /*tp_name*/
    sizeof(DocumentViewKey1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentViewKey1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentViewKey1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentViewKey1_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentViewKey1_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentViewKey1_new,                 /* tp_new */
};

static PyTypeObject DocumentViewType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.DocumentView",             /*tp_name*/
    sizeof(DocumentView),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentView_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentView objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentView_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentView_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentView_new,                 /* tp_new */
};

static PyTypeObject DocumentDataKey0Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.DocumentDataKey0",             /*tp_name*/
    sizeof(DocumentDataKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentDataKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentDataKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentDataKey0_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentDataKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentDataKey0_new,                 /* tp_new */
};

static PyTypeObject DocumentDataType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.DocumentData",             /*tp_name*/
    sizeof(DocumentData),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentData_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentData objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentData_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentData_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentData_new,                 /* tp_new */
};

static PyTypeObject ProductType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.Product",             /*tp_name*/
    sizeof(Product),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Product_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Product objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Product_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Product_init,      /* tp_init */
    0,                         /* tp_alloc */
    Product_new,                 /* tp_new */
};

static PyTypeObject ProductLinkKey0Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.ProductLinkKey0",             /*tp_name*/
    sizeof(ProductLinkKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ProductLinkKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ProductLinkKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ProductLinkKey0_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ProductLinkKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    ProductLinkKey0_new,                 /* tp_new */
};

static PyTypeObject ProductLinkKey1Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.ProductLinkKey1",             /*tp_name*/
    sizeof(ProductLinkKey1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ProductLinkKey1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ProductLinkKey1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ProductLinkKey1_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ProductLinkKey1_init,      /* tp_init */
    0,                         /* tp_alloc */
    ProductLinkKey1_new,                 /* tp_new */
};

static PyTypeObject ProductLinkType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.ProductLink",             /*tp_name*/
    sizeof(ProductLink),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ProductLink_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ProductLink objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ProductLink_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ProductLink_init,      /* tp_init */
    0,                         /* tp_alloc */
    ProductLink_new,                 /* tp_new */
};

static PyTypeObject ProductViewKey0Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.ProductViewKey0",             /*tp_name*/
    sizeof(ProductViewKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ProductViewKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ProductViewKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ProductViewKey0_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ProductViewKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    ProductViewKey0_new,                 /* tp_new */
};

static PyTypeObject ProductViewKey1Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.ProductViewKey1",             /*tp_name*/
    sizeof(ProductViewKey1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ProductViewKey1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ProductViewKey1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ProductViewKey1_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ProductViewKey1_init,      /* tp_init */
    0,                         /* tp_alloc */
    ProductViewKey1_new,                 /* tp_new */
};

static PyTypeObject ProductViewType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.ProductView",             /*tp_name*/
    sizeof(ProductView),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ProductView_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ProductView objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ProductView_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ProductView_init,      /* tp_init */
    0,                         /* tp_alloc */
    ProductView_new,                 /* tp_new */
};

static PyTypeObject ProductDataKey0Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.ProductDataKey0",             /*tp_name*/
    sizeof(ProductDataKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ProductDataKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ProductDataKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ProductDataKey0_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ProductDataKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    ProductDataKey0_new,                 /* tp_new */
};

static PyTypeObject ProductDataType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.ProductData",             /*tp_name*/
    sizeof(ProductData),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ProductData_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ProductData objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ProductData_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ProductData_init,      /* tp_init */
    0,                         /* tp_alloc */
    ProductData_new,                 /* tp_new */
};

static PyTypeObject BarcodeKey0Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.BarcodeKey0",             /*tp_name*/
    sizeof(BarcodeKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)BarcodeKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "BarcodeKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    BarcodeKey0_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)BarcodeKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    BarcodeKey0_new,                 /* tp_new */
};

static PyTypeObject BarcodeKey1Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.BarcodeKey1",             /*tp_name*/
    sizeof(BarcodeKey1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)BarcodeKey1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "BarcodeKey1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    BarcodeKey1_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)BarcodeKey1_init,      /* tp_init */
    0,                         /* tp_alloc */
    BarcodeKey1_new,                 /* tp_new */
};

static PyTypeObject BarcodeType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.Barcode",             /*tp_name*/
    sizeof(Barcode),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Barcode_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Barcode objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Barcode_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Barcode_init,      /* tp_init */
    0,                         /* tp_alloc */
    Barcode_new,                 /* tp_new */
};

static PyTypeObject AccountKey0Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.AccountKey0",             /*tp_name*/
    sizeof(AccountKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)AccountKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "AccountKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    AccountKey0_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)AccountKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    AccountKey0_new,                 /* tp_new */
};

static PyTypeObject AccountType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.Account",             /*tp_name*/
    sizeof(Account),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Account_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Account objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Account_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Account_init,      /* tp_init */
    0,                         /* tp_alloc */
    Account_new,                 /* tp_new */
};

static PyTypeObject SubaccountKey0Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.SubaccountKey0",             /*tp_name*/
    sizeof(SubaccountKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)SubaccountKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "SubaccountKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    SubaccountKey0_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)SubaccountKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    SubaccountKey0_new,                 /* tp_new */
};

static PyTypeObject SubaccountType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.Subaccount",             /*tp_name*/
    sizeof(Subaccount),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Subaccount_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Subaccount objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Subaccount_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Subaccount_init,      /* tp_init */
    0,                         /* tp_alloc */
    Subaccount_new,                 /* tp_new */
};

static PyTypeObject ClassType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.Class",             /*tp_name*/
    sizeof(Class),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Class_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Class objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Class_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Class_init,      /* tp_init */
    0,                         /* tp_alloc */
    Class_new,                 /* tp_new */
};

static PyTypeObject GroupKey0Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.GroupKey0",             /*tp_name*/
    sizeof(GroupKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)GroupKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "GroupKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    GroupKey0_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)GroupKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    GroupKey0_new,                 /* tp_new */
};

static PyTypeObject GroupKey1Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.GroupKey1",             /*tp_name*/
    sizeof(GroupKey1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)GroupKey1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "GroupKey1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    GroupKey1_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)GroupKey1_init,      /* tp_init */
    0,                         /* tp_alloc */
    GroupKey1_new,                 /* tp_new */
};

static PyTypeObject GroupType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.Group",             /*tp_name*/
    sizeof(Group),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Group_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Group objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Group_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Group_init,      /* tp_init */
    0,                         /* tp_alloc */
    Group_new,                 /* tp_new */
};

static PyTypeObject SubgroupKey0Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.SubgroupKey0",             /*tp_name*/
    sizeof(SubgroupKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)SubgroupKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "SubgroupKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    SubgroupKey0_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)SubgroupKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    SubgroupKey0_new,                 /* tp_new */
};

static PyTypeObject SubgroupKey1Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.SubgroupKey1",             /*tp_name*/
    sizeof(SubgroupKey1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)SubgroupKey1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "SubgroupKey1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    SubgroupKey1_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)SubgroupKey1_init,      /* tp_init */
    0,                         /* tp_alloc */
    SubgroupKey1_new,                 /* tp_new */
};

static PyTypeObject SubgroupType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.Subgroup",             /*tp_name*/
    sizeof(Subgroup),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Subgroup_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Subgroup objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Subgroup_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Subgroup_init,      /* tp_init */
    0,                         /* tp_alloc */
    Subgroup_new,                 /* tp_new */
};

static PyTypeObject RegionKey0Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.RegionKey0",             /*tp_name*/
    sizeof(RegionKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)RegionKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "RegionKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    RegionKey0_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)RegionKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    RegionKey0_new,                 /* tp_new */
};

static PyTypeObject RegionKey1Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.RegionKey1",             /*tp_name*/
    sizeof(RegionKey1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)RegionKey1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "RegionKey1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    RegionKey1_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)RegionKey1_init,      /* tp_init */
    0,                         /* tp_alloc */
    RegionKey1_new,                 /* tp_new */
};

static PyTypeObject RegionType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.Region",             /*tp_name*/
    sizeof(Region),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Region_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Region objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Region_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Region_init,      /* tp_init */
    0,                         /* tp_alloc */
    Region_new,                 /* tp_new */
};

static PyTypeObject OtdelKey0Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.OtdelKey0",             /*tp_name*/
    sizeof(OtdelKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)OtdelKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "OtdelKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    OtdelKey0_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)OtdelKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    OtdelKey0_new,                 /* tp_new */
};

static PyTypeObject OtdelType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.Otdel",             /*tp_name*/
    sizeof(Otdel),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Otdel_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Otdel objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Otdel_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Otdel_init,      /* tp_init */
    0,                         /* tp_alloc */
    Otdel_new,                 /* tp_new */
};

static PyTypeObject PartnerKey0Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.PartnerKey0",             /*tp_name*/
    sizeof(PartnerKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PartnerKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "PartnerKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    PartnerKey0_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)PartnerKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    PartnerKey0_new,                 /* tp_new */
};

static PyTypeObject PartnerKey1Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.PartnerKey1",             /*tp_name*/
    sizeof(PartnerKey1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PartnerKey1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "PartnerKey1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    PartnerKey1_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)PartnerKey1_init,      /* tp_init */
    0,                         /* tp_alloc */
    PartnerKey1_new,                 /* tp_new */
};

static PyTypeObject PartnerKey2Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.PartnerKey2",             /*tp_name*/
    sizeof(PartnerKey2),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PartnerKey2_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "PartnerKey2 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    PartnerKey2_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)PartnerKey2_init,      /* tp_init */
    0,                         /* tp_alloc */
    PartnerKey2_new,                 /* tp_new */
};

static PyTypeObject PartnerKey3Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.PartnerKey3",             /*tp_name*/
    sizeof(PartnerKey3),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PartnerKey3_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "PartnerKey3 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    PartnerKey3_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)PartnerKey3_init,      /* tp_init */
    0,                         /* tp_alloc */
    PartnerKey3_new,                 /* tp_new */
};

static PyTypeObject PartnerType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.Partner",             /*tp_name*/
    sizeof(Partner),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Partner_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Partner objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Partner_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Partner_init,      /* tp_init */
    0,                         /* tp_alloc */
    Partner_new,                 /* tp_new */
};

static PyTypeObject PartnerDataKey0Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.PartnerDataKey0",             /*tp_name*/
    sizeof(PartnerDataKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PartnerDataKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "PartnerDataKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    PartnerDataKey0_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)PartnerDataKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    PartnerDataKey0_new,                 /* tp_new */
};

static PyTypeObject PartnerDataKey1Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.PartnerDataKey1",             /*tp_name*/
    sizeof(PartnerDataKey1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PartnerDataKey1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "PartnerDataKey1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    PartnerDataKey1_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)PartnerDataKey1_init,      /* tp_init */
    0,                         /* tp_alloc */
    PartnerDataKey1_new,                 /* tp_new */
};

static PyTypeObject PartnerDataType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.PartnerData",             /*tp_name*/
    sizeof(PartnerData),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PartnerData_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "PartnerData objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    PartnerData_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)PartnerData_init,      /* tp_init */
    0,                         /* tp_alloc */
    PartnerData_new,                 /* tp_new */
};

static PyTypeObject PartnerViewKey0Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.PartnerViewKey0",             /*tp_name*/
    sizeof(PartnerViewKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PartnerViewKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "PartnerViewKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    PartnerViewKey0_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)PartnerViewKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    PartnerViewKey0_new,                 /* tp_new */
};

static PyTypeObject PartnerViewKey1Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.PartnerViewKey1",             /*tp_name*/
    sizeof(PartnerViewKey1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PartnerViewKey1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "PartnerViewKey1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    PartnerViewKey1_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)PartnerViewKey1_init,      /* tp_init */
    0,                         /* tp_alloc */
    PartnerViewKey1_new,                 /* tp_new */
};

static PyTypeObject PartnerViewType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.PartnerView",             /*tp_name*/
    sizeof(PartnerView),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PartnerView_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "PartnerView objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    PartnerView_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)PartnerView_init,      /* tp_init */
    0,                         /* tp_alloc */
    PartnerView_new,                 /* tp_new */
};

static PyTypeObject SkladKey0Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.SkladKey0",             /*tp_name*/
    sizeof(SkladKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)SkladKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "SkladKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    SkladKey0_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)SkladKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    SkladKey0_new,                 /* tp_new */
};

static PyTypeObject SkladType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.Sklad",             /*tp_name*/
    sizeof(Sklad),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Sklad_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Sklad objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Sklad_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Sklad_init,      /* tp_init */
    0,                         /* tp_alloc */
    Sklad_new,                 /* tp_new */
};

static PyTypeObject DocumentProwKey0Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.DocumentProwKey0",             /*tp_name*/
    sizeof(DocumentProwKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentProwKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentProwKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentProwKey0_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentProwKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentProwKey0_new,                 /* tp_new */
};

static PyTypeObject DocumentProwType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.DocumentProw",             /*tp_name*/
    sizeof(DocumentProw),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentProw_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentProw objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentProw_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentProw_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentProw_new,                 /* tp_new */
};

static PyTypeObject DocumentTypeKey0Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.DocumentTypeKey0",             /*tp_name*/
    sizeof(DocumentTypeKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentTypeKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentTypeKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentTypeKey0_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentTypeKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentTypeKey0_new,                 /* tp_new */
};

static PyTypeObject DocumentTypeType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.DocumentType",             /*tp_name*/
    sizeof(DocumentType),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentType_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentType objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentType_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentType_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentType_new,                 /* tp_new */
};

static PyTypeObject StockKey0Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.StockKey0",             /*tp_name*/
    sizeof(StockKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)StockKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "StockKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    StockKey0_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)StockKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    StockKey0_new,                 /* tp_new */
};

static PyTypeObject StockKey1Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.StockKey1",             /*tp_name*/
    sizeof(StockKey1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)StockKey1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "StockKey1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    StockKey1_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)StockKey1_init,      /* tp_init */
    0,                         /* tp_alloc */
    StockKey1_new,                 /* tp_new */
};

static PyTypeObject StockType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.Stock",             /*tp_name*/
    sizeof(Stock),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Stock_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Stock objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Stock_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Stock_init,      /* tp_init */
    0,                         /* tp_alloc */
    Stock_new,                 /* tp_new */
};

static PyTypeObject ProwodKey0Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.ProwodKey0",             /*tp_name*/
    sizeof(ProwodKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ProwodKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ProwodKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ProwodKey0_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ProwodKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    ProwodKey0_new,                 /* tp_new */
};

static PyTypeObject ProwodKey1Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.ProwodKey1",             /*tp_name*/
    sizeof(ProwodKey1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ProwodKey1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ProwodKey1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ProwodKey1_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ProwodKey1_init,      /* tp_init */
    0,                         /* tp_alloc */
    ProwodKey1_new,                 /* tp_new */
};

static PyTypeObject ProwodKey2Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.ProwodKey2",             /*tp_name*/
    sizeof(ProwodKey2),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ProwodKey2_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ProwodKey2 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ProwodKey2_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ProwodKey2_init,      /* tp_init */
    0,                         /* tp_alloc */
    ProwodKey2_new,                 /* tp_new */
};

static PyTypeObject ProwodKey3Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.ProwodKey3",             /*tp_name*/
    sizeof(ProwodKey3),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ProwodKey3_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ProwodKey3 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ProwodKey3_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ProwodKey3_init,      /* tp_init */
    0,                         /* tp_alloc */
    ProwodKey3_new,                 /* tp_new */
};

static PyTypeObject ProwodKey4Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.ProwodKey4",             /*tp_name*/
    sizeof(ProwodKey4),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ProwodKey4_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ProwodKey4 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ProwodKey4_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ProwodKey4_init,      /* tp_init */
    0,                         /* tp_alloc */
    ProwodKey4_new,                 /* tp_new */
};

static PyTypeObject ProwodType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.Prowod",             /*tp_name*/
    sizeof(Prowod),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Prowod_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Prowod objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Prowod_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Prowod_init,      /* tp_init */
    0,                         /* tp_alloc */
    Prowod_new,                 /* tp_new */
};

static PyTypeObject Prowod2Key0Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.Prowod2Key0",             /*tp_name*/
    sizeof(Prowod2Key0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Prowod2Key0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Prowod2Key0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Prowod2Key0_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Prowod2Key0_init,      /* tp_init */
    0,                         /* tp_alloc */
    Prowod2Key0_new,                 /* tp_new */
};

static PyTypeObject Prowod2Key1Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.Prowod2Key1",             /*tp_name*/
    sizeof(Prowod2Key1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Prowod2Key1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Prowod2Key1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Prowod2Key1_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Prowod2Key1_init,      /* tp_init */
    0,                         /* tp_alloc */
    Prowod2Key1_new,                 /* tp_new */
};

static PyTypeObject Prowod2Key2Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.Prowod2Key2",             /*tp_name*/
    sizeof(Prowod2Key2),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Prowod2Key2_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Prowod2Key2 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Prowod2Key2_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Prowod2Key2_init,      /* tp_init */
    0,                         /* tp_alloc */
    Prowod2Key2_new,                 /* tp_new */
};

static PyTypeObject Prowod2Key3Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.Prowod2Key3",             /*tp_name*/
    sizeof(Prowod2Key3),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Prowod2Key3_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Prowod2Key3 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Prowod2Key3_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Prowod2Key3_init,      /* tp_init */
    0,                         /* tp_alloc */
    Prowod2Key3_new,                 /* tp_new */
};

static PyTypeObject Prowod2Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.Prowod2",             /*tp_name*/
    sizeof(Prowod2),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Prowod2_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Prowod2 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Prowod2_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Prowod2_init,      /* tp_init */
    0,                         /* tp_alloc */
    Prowod2_new,                 /* tp_new */
};

static PyTypeObject SaldoKey0Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.SaldoKey0",             /*tp_name*/
    sizeof(SaldoKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)SaldoKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "SaldoKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    SaldoKey0_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)SaldoKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    SaldoKey0_new,                 /* tp_new */
};

static PyTypeObject SaldoType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.Saldo",             /*tp_name*/
    sizeof(Saldo),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Saldo_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Saldo objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Saldo_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Saldo_init,      /* tp_init */
    0,                         /* tp_alloc */
    Saldo_new,                 /* tp_new */
};

static PyTypeObject ProtocolKey0Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.ProtocolKey0",             /*tp_name*/
    sizeof(ProtocolKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ProtocolKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ProtocolKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ProtocolKey0_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ProtocolKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    ProtocolKey0_new,                 /* tp_new */
};

static PyTypeObject ProtocolKey1Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.ProtocolKey1",             /*tp_name*/
    sizeof(ProtocolKey1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ProtocolKey1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ProtocolKey1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ProtocolKey1_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ProtocolKey1_init,      /* tp_init */
    0,                         /* tp_alloc */
    ProtocolKey1_new,                 /* tp_new */
};

static PyTypeObject ProtocolKey2Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.ProtocolKey2",             /*tp_name*/
    sizeof(ProtocolKey2),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ProtocolKey2_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ProtocolKey2 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ProtocolKey2_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ProtocolKey2_init,      /* tp_init */
    0,                         /* tp_alloc */
    ProtocolKey2_new,                 /* tp_new */
};

static PyTypeObject ProtocolType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.Protocol",             /*tp_name*/
    sizeof(Protocol),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Protocol_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Protocol objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Protocol_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Protocol_init,      /* tp_init */
    0,                         /* tp_alloc */
    Protocol_new,                 /* tp_new */
};

static PyTypeObject UserKey0Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.UserKey0",             /*tp_name*/
    sizeof(UserKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)UserKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "UserKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    UserKey0_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)UserKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    UserKey0_new,                 /* tp_new */
};

static PyTypeObject UserType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.User",             /*tp_name*/
    sizeof(User),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)User_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "User objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    User_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)User_init,      /* tp_init */
    0,                         /* tp_alloc */
    User_new,                 /* tp_new */
};

static PyTypeObject ShiftKey0Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.ShiftKey0",             /*tp_name*/
    sizeof(ShiftKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ShiftKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ShiftKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ShiftKey0_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ShiftKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    ShiftKey0_new,                 /* tp_new */
};

static PyTypeObject ShiftKey1Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.ShiftKey1",             /*tp_name*/
    sizeof(ShiftKey1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ShiftKey1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ShiftKey1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ShiftKey1_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ShiftKey1_init,      /* tp_init */
    0,                         /* tp_alloc */
    ShiftKey1_new,                 /* tp_new */
};

static PyTypeObject ShiftKey2Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.ShiftKey2",             /*tp_name*/
    sizeof(ShiftKey2),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ShiftKey2_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ShiftKey2 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ShiftKey2_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ShiftKey2_init,      /* tp_init */
    0,                         /* tp_alloc */
    ShiftKey2_new,                 /* tp_new */
};

static PyTypeObject ShiftKey3Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.ShiftKey3",             /*tp_name*/
    sizeof(ShiftKey3),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ShiftKey3_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ShiftKey3 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ShiftKey3_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ShiftKey3_init,      /* tp_init */
    0,                         /* tp_alloc */
    ShiftKey3_new,                 /* tp_new */
};

static PyTypeObject ShiftType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.Shift",             /*tp_name*/
    sizeof(Shift),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Shift_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Shift objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Shift_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Shift_init,      /* tp_init */
    0,                         /* tp_alloc */
    Shift_new,                 /* tp_new */
};

static PyTypeObject ChecksumKey0Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.ChecksumKey0",             /*tp_name*/
    sizeof(ChecksumKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ChecksumKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ChecksumKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ChecksumKey0_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ChecksumKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    ChecksumKey0_new,                 /* tp_new */
};

static PyTypeObject ChecksumKey1Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.ChecksumKey1",             /*tp_name*/
    sizeof(ChecksumKey1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ChecksumKey1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ChecksumKey1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ChecksumKey1_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ChecksumKey1_init,      /* tp_init */
    0,                         /* tp_alloc */
    ChecksumKey1_new,                 /* tp_new */
};

static PyTypeObject ChecksumType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.Checksum",             /*tp_name*/
    sizeof(Checksum),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Checksum_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Checksum objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Checksum_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Checksum_init,      /* tp_init */
    0,                         /* tp_alloc */
    Checksum_new,                 /* tp_new */
};

static PyTypeObject CheckKey0Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.CheckKey0",             /*tp_name*/
    sizeof(CheckKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)CheckKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "CheckKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    CheckKey0_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)CheckKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    CheckKey0_new,                 /* tp_new */
};

static PyTypeObject CheckKey1Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.CheckKey1",             /*tp_name*/
    sizeof(CheckKey1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)CheckKey1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "CheckKey1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    CheckKey1_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)CheckKey1_init,      /* tp_init */
    0,                         /* tp_alloc */
    CheckKey1_new,                 /* tp_new */
};

static PyTypeObject CheckKey2Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.CheckKey2",             /*tp_name*/
    sizeof(CheckKey2),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)CheckKey2_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "CheckKey2 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    CheckKey2_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)CheckKey2_init,      /* tp_init */
    0,                         /* tp_alloc */
    CheckKey2_new,                 /* tp_new */
};

static PyTypeObject CheckType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.Check",             /*tp_name*/
    sizeof(Check),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Check_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Check objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Check_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Check_init,      /* tp_init */
    0,                         /* tp_alloc */
    Check_new,                 /* tp_new */
};

static PyTypeObject RealizationKey0Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.RealizationKey0",             /*tp_name*/
    sizeof(RealizationKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)RealizationKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "RealizationKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    RealizationKey0_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)RealizationKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    RealizationKey0_new,                 /* tp_new */
};

static PyTypeObject RealizationKey1Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.RealizationKey1",             /*tp_name*/
    sizeof(RealizationKey1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)RealizationKey1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "RealizationKey1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    RealizationKey1_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)RealizationKey1_init,      /* tp_init */
    0,                         /* tp_alloc */
    RealizationKey1_new,                 /* tp_new */
};

static PyTypeObject RealizationKey2Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.RealizationKey2",             /*tp_name*/
    sizeof(RealizationKey2),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)RealizationKey2_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "RealizationKey2 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    RealizationKey2_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)RealizationKey2_init,      /* tp_init */
    0,                         /* tp_alloc */
    RealizationKey2_new,                 /* tp_new */
};

static PyTypeObject RealizationKey3Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.RealizationKey3",             /*tp_name*/
    sizeof(RealizationKey3),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)RealizationKey3_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "RealizationKey3 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    RealizationKey3_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)RealizationKey3_init,      /* tp_init */
    0,                         /* tp_alloc */
    RealizationKey3_new,                 /* tp_new */
};

static PyTypeObject RealizationKey4Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.RealizationKey4",             /*tp_name*/
    sizeof(RealizationKey4),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)RealizationKey4_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "RealizationKey4 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    RealizationKey4_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)RealizationKey4_init,      /* tp_init */
    0,                         /* tp_alloc */
    RealizationKey4_new,                 /* tp_new */
};

static PyTypeObject RealizationType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.Realization",             /*tp_name*/
    sizeof(Realization),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Realization_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Realization objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Realization_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Realization_init,      /* tp_init */
    0,                         /* tp_alloc */
    Realization_new,                 /* tp_new */
};

static PyTypeObject DiscountKey0Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.DiscountKey0",             /*tp_name*/
    sizeof(DiscountKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DiscountKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DiscountKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DiscountKey0_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DiscountKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    DiscountKey0_new,                 /* tp_new */
};

static PyTypeObject DiscountKey1Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.DiscountKey1",             /*tp_name*/
    sizeof(DiscountKey1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DiscountKey1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DiscountKey1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DiscountKey1_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DiscountKey1_init,      /* tp_init */
    0,                         /* tp_alloc */
    DiscountKey1_new,                 /* tp_new */
};

static PyTypeObject DiscountType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.Discount",             /*tp_name*/
    sizeof(Discount),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Discount_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Discount objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Discount_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Discount_init,      /* tp_init */
    0,                         /* tp_alloc */
    Discount_new,                 /* tp_new */
};

static PyTypeObject StructFileKey0Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.StructFileKey0",             /*tp_name*/
    sizeof(StructFileKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)StructFileKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "StructFileKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    StructFileKey0_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)StructFileKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    StructFileKey0_new,                 /* tp_new */
};

static PyTypeObject StructFileKey1Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.StructFileKey1",             /*tp_name*/
    sizeof(StructFileKey1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)StructFileKey1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "StructFileKey1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    StructFileKey1_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)StructFileKey1_init,      /* tp_init */
    0,                         /* tp_alloc */
    StructFileKey1_new,                 /* tp_new */
};

static PyTypeObject StructFileType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "domino.StructFile",             /*tp_name*/
    sizeof(StructFile),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)StructFile_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "StructFile objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    StructFile_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)StructFile_init,      /* tp_init */
    0,                         /* tp_alloc */
    StructFile_new,                 /* tp_new */
};

PyMODINIT_FUNC initdomino(void)
{
    PyObject* m;

    if (PyType_Ready(&AliasType) < 0)
        return;
    setlocale(LC_ALL, "");
    do_log_set_log_func(log_func, NULL);
    LOG("domino init\n");
    domino_init(NULL, TRUE, DO_PARAM_END);

    m = Py_InitModule3("domino", module_methods,
                       "Example module that creates an extension type.");

    if (m == NULL)
        return;

    DominoError = PyErr_NewException("domino.error", NULL, NULL);
    Py_INCREF(DominoError);
    PyModule_AddObject(m, "error", DominoError);

    Py_INCREF(&AliasType);
    PyModule_AddObject(m, "Alias", (PyObject *)&AliasType);
}
