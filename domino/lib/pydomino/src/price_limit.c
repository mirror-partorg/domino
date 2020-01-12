
#include "price_limit.h"
#include <datetime.h>
#include "../var/db.h"
#include "../var/util.h"

static void PriceLimit_dealloc(PyObject* self)
{
    LOG("PriceLimit free\n");
    if ( ((PriceLimit*)self)->info )
        do_rz_info_free(((PriceLimit*)self)->info);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *PriceLimit_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("PriceLimit new\n");
    PriceLimit *self;
    self = PyObject_NEW(PriceLimit, type);
    //self = (PriceLimit *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    return (PyObject *)self;
}
static int PriceLimit_init(PriceLimit *self, PyObject *args, PyObject *kwds)
{
    LOG("PriceLimit init\n");
    static char *kwlist[] = {"key", NULL};
    PyObject *key = NULL;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &key) ) {
        do_log(LOG_ERR, "no param \"key\"");
        return -1;
    }
    if ( key ) {
        if ( Py_TYPE(key) == getDocumentOrderType() ) {
            DocumentOrder *order=(DocumentOrder *)key;
            self->info = do_rz_info_new_from_document_order(order->alias->alias, order->priv);
        }
        else
        if ( Py_TYPE(key) == getDocumentOrderKey0Type() ) {
            DocumentOrderKey0 *order=(DocumentOrderKey0 *)key;
            self->info = do_rz_info_new_from_document_order_key0(order->alias->alias, order->priv);
        }
        else {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return -1;

        }
    }
    PyDateTime_IMPORT;
    return 0;
}

static PyObject *PriceLimit_barcode(PriceLimit *self, void *unused)
{
    return MyString_FromString(self->info->barcode);
}
static PyObject *PriceLimit_src_line(PriceLimit *self, void *unused)
{
    char *value;
    PyObject *res;
    value = do_strdup_printf("%s.%s.%s.%d", self->info->key.dtype, self->info->key.sklad, self->info->key.document, self->info->key.line);
    res=MyString_FromString(value);
    do_free(value);
    return res;
}
static PyObject *PriceLimit_inn(PriceLimit *self, void *unused)
{
    return MyString_FromString(self->info->mnn);
}
static PyObject *PriceLimit_trade_name(PriceLimit *self, void *unused)
{
    return MyString_FromString(self->info->trade_name);
}
static PyObject *PriceLimit_pack_form(PriceLimit *self, void *unused)
{
    return MyString_FromString(self->info->pack_form);
}
static PyObject *PriceLimit_manufactor(PriceLimit *self, void *unused)
{
    return MyString_FromString(self->info->manufactor);
}
static PyObject *PriceLimit_pack_quant(PriceLimit *self, void *unused)
{
    return MyLong_FromLong(self->info->pack_quant);
}
static PyObject *PriceLimit_price(PriceLimit *self, void *unused)
{
    return PyFloat_FromDouble(self->info->price);
}
static PyObject *PriceLimit_unpack(PriceLimit *self, void *unused)
{
    return PyBool_FromLong(self->info->unpack);
}
static PyObject *PriceLimit_reg_number(PriceLimit *self, void *unused)
{
    return MyString_FromString(self->info->order.number);
}
static PyObject *PriceLimit_reg_date(PriceLimit *self, void *unused)
{

    //return PyLong_FromLong(self->info->order.time);
    struct tm tm;
    struct tm *p;
    time_t time;
    time = self->info->order.time;
    //tm = *gmtime(&self->info->order.time);
    p = localtime(&time);
    char *buf;
    if ( !p ) {
         return PyDate_FromDate(1970,1,1);
    }
    else {
       tm = *p;
       return PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);
    }
}
static PyGetSetDef PriceLimit_getset[] = {
    {"barcode", (getter)PriceLimit_barcode},
    {"inn", (getter)PriceLimit_inn},
    {"trade_name", (getter)PriceLimit_trade_name},
    {"pack_form", (getter)PriceLimit_pack_form},
    {"pack_quant", (getter)PriceLimit_pack_quant},
    {"manufactor", (getter)PriceLimit_manufactor},
    {"price", (getter)PriceLimit_price},
    {"unpack", (getter)PriceLimit_unpack},
    {"reg_number", (getter)PriceLimit_reg_number},
    {"reg_date", (getter)PriceLimit_reg_date},
    {"src_line", (getter)PriceLimit_src_line},
    {NULL}
};

static PyMethodDef PriceLimit_methods[] = {
    //{"rest", (PyCFunction)PriceLimit_rest, METH_VARARGS|METH_KEYWORDS, "set"},

    {NULL}
};
static PyObject *PriceLimit_str(PriceLimit* self, PyObject *args, PyObject *kwds)
{
    char *buf, *doc;
    char time[100];
    struct tm tm;
    struct tm *p;
    PyObject *res;
    time_t t1;
    t1=self->info->order.time;

    
    p = localtime(&t1);
    if ( p ) {
        tm = *p;
        strftime(time, sizeof(time)-1, "%d.%m.%Y",&tm);
    }
    else 
        time[0]='\0';
    doc = do_strdup_printf("%s от %s", self->info->order.number, time);
    buf = do_strdup_printf(
    "Рег.цена\nМНН:%s\nНАИМЕНОВАНИЕ:%s\nФОРМА:%s\n№: %d\nПРОИЗВОДИТЕЛЬ:%s\nЦЕНА:%f\nШТРИХКОД:%s\nПЕРВИЧНАЯ УПАКОВКА:%s\nДОКУМЕНТ:%d",
    self->info->mnn,
    self->info->trade_name,
    self->info->pack_form,
    self->info->pack_quant,
    self->info->manufactor,
    self->info->price,
    self->info->barcode,
    !self->info->unpack ? "Да":"Нет",
    doc);

    res = MyString_FromString(buf);
    do_free(buf);do_free(doc);
    return res;
}

static PyTypeObject PriceLimitType_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.PriceLimit",             /*tp_name*/
    sizeof(PriceLimit),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PriceLimit_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)PriceLimit_str,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "PriceLimit objects",           /* tp_doc */
    0,0,
    0,0,0,0,               /* tp_iternext */
    PriceLimit_methods,             /* tp_methods */
    0,
    PriceLimit_getset,      /*tp_getset */
    0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)PriceLimit_init,      /* tp_init */
    0,                         /* tp_alloc */
    PriceLimit_new,                 /* tp_new */
};
PyTypeObject *getPriceLimitType()
{
    return &PriceLimitType_;
}
