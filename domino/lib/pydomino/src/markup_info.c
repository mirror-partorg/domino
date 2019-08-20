
#include "markup_info.h"
#include <datetime.h>
#include "../var/db.h"
#include "../var/util.h"

static void MarkupInfo_dealloc(PyObject* self)
{
    LOG("MarkupInfo free\n");
    if ( ((MarkupInfo*)self)->info )
        do_markup_free(((MarkupInfo*)self)->info);
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *MarkupInfo_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("MarkupInfo new\n");
    MarkupInfo *self;
    self = PyObject_NEW(MarkupInfo, type);
    //self = (MarkupInfo *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    return (PyObject *)self;
}
static int MarkupInfo_init(MarkupInfo *self, PyObject *args, PyObject *kwds)
{
    LOG("MarkupInfo init\n");
    PyDateTime_IMPORT;
    return 0;
}

static PyObject *MarkupInfo_price_reg(MarkupInfo *self, void *unused)
{
    return PyBool_FromLong(self->info->price_reg);
}
static PyObject *MarkupInfo_aver_off(MarkupInfo *self, void *unused)
{
    return PyBool_FromLong(!self->info->aver_off);
}
static PyObject *MarkupInfo_round_up(MarkupInfo *self, void *unused)
{
    return PyBool_FromLong(self->info->round_up);
}
static PyObject *MarkupInfo_price(MarkupInfo *self, void *unused)
{
    if ( self->info->price_define )
        return PyFloat_FromDouble(self->info->price);
    else {
        Py_INCREF(Py_None);
        return Py_None;
    }
}
static PyObject *MarkupInfo_scale(MarkupInfo *self, void *unused)
{
    if ( self->info->price_define ) {
        Py_INCREF(Py_None);
        return Py_None;
    }
    PyObject *scale_list;
    scale_list = PyList_New(0);
    int i;
    for ( i = 0; i < self->info->scale->count; i+=2 ) {
        double price = *((double*)self->info->scale->list[i]);
        double rate = *((double*)self->info->scale->list[i+1]);
        PyObject *item;
        item = PyTuple_New(2);
        PyTuple_SET_ITEM(item, 0, PyFloat_FromDouble(price));
        PyTuple_SET_ITEM(item, 1, PyFloat_FromDouble(rate));
        PyList_Append(scale_list, (PyObject*)item);
        Py_DECREF(item);
    }
    return scale_list;
}
static PyObject *MarkupInfo_scale_max(MarkupInfo *self, void *unused)
{
    PyObject *max_list;
    if ( !self->info->scale_max || !self->info->scale_max->count ) {
        Py_INCREF(Py_None);
        return Py_None;
    }
    max_list = PyList_New(0);
    int i;
    for ( i = 0; i < self->info->scale_max->count; i+=2 ) {
        double price = *((double*)self->info->scale_max->list[i]);
        double rate = *((double*)self->info->scale_max->list[i+1]);
        PyObject *item;
        item = PyTuple_New(2);
        PyTuple_SET_ITEM(item, 0, PyFloat_FromDouble(price));
        PyTuple_SET_ITEM(item, 1, PyFloat_FromDouble(rate));
        PyList_Append(max_list, (PyObject*)item);
        Py_DECREF(item);
    }
    return max_list;
}
static PyGetSetDef MarkupInfo_getset[] = {
    {"regularized", (getter)MarkupInfo_price_reg},
    {"averaged", (getter)MarkupInfo_aver_off},
    {"is_ceil", (getter)MarkupInfo_round_up},
    {"price", (getter)MarkupInfo_price},
    {"scale", (getter)MarkupInfo_scale},
    {"scale_max", (getter)MarkupInfo_scale_max},
    {NULL}
};

static PyMethodDef MarkupInfo_methods[] = {
    //{"rest", (PyCFunction)MarkupInfo_rest, METH_VARARGS|METH_KEYWORDS, "set"},

    {NULL}
};
static PyObject *MarkupInfo_str(MarkupInfo* self, PyObject *args, PyObject *kwds)
{
    char *buf, *scale_max=NULL, *scale;
    PyObject *res;
    
    if ( self->info->price_define ) {
        scale = do_strdup_printf("Цена: %.2f", self->info->price);
    }
    else {
        int i;
        if ( self->info->scale->count < 3 ) {
            double rate = *((double*)self->info->scale->list[1]);
            scale = do_strdup_printf("Наценка: %.3f%%", rate);
        }
        else {
            double bottom=0;
            
            scale = do_strdup("Наценка:");
            for ( i = 0; i < self->info->scale->count; i+=2 ) {
               double price = *((double*)self->info->scale->list[i]);
               double rate = *((double*)self->info->scale->list[i+1]);
               char *str,*buf;
               if ( price > 0 ) {
                   if ( !bottom ) 
                       buf = do_strdup_printf("[до %.2f: %.3f%%", price, rate);
                   else
                       buf = do_strdup_printf("с %.2f до %.2f: %.3f%%", bottom, price, rate);
               }
               else {
                   buf = do_strdup_printf("свыше: %.3f%%]", rate);
               }
               bottom=price;
               str=do_strdup_printf("%s %s",scale,buf);
               do_free(scale); do_free(buf);
               scale=str;
            }
            
        }
    }
    
    if ( self->info->scale_max && self->info->scale_max->count ) {
        int i;
        if ( self->info->scale_max->count < 3 ) {
            double rate = *((double*)self->info->scale_max->list[1]);
            scale_max = do_strdup_printf("Макс.наценка: %.3f%%", rate);
        }
        else {
            double bottom=0;
            scale_max = do_strdup("Макс.наценка:");
            for ( i = 0; i < self->info->scale_max->count; i+=2 ) {
               double price = *((double*)self->info->scale_max->list[i]);
               double rate = *((double*)self->info->scale_max->list[i+1]);
               char *str,*buf;
               if ( price > 0 ) {
                   if ( !bottom ) 
                       buf = do_strdup_printf("[до %.2f: %.3f%%", price, rate);
                   else
                       buf = do_strdup_printf("с %.2f до %.2f: %.3f%%", bottom, price, rate);
               }
               else {
                   buf = do_strdup_printf("свыше: %.3f%%]", rate);
               }
               bottom=price;
               str=do_strdup_printf("%s %s",scale_max,buf);
               do_free(scale_max); do_free(buf);
               scale_max=str;
            }
        };
    }
    buf = do_strdup_printf(
    "%s%s%s%s%c%s\n",
    self->info->price_reg ? "Регулируемая\n":"",
    !self->info->aver_off ? "Усреднение\n":"",
    self->info->round_up ? "Округление вверх\n":"",
    scale,
    (scale_max) ? '\n': ' ',
    (scale_max) ? scale_max : "");

    res = MyString_FromString(buf);
    do_free(buf);do_free(scale); if ( scale_max ) do_free(scale_max);
    return res;
}

static PyTypeObject MarkupInfoType_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.MarkupInfo",             /*tp_name*/
    sizeof(MarkupInfo),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)MarkupInfo_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)MarkupInfo_str,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "MarkupInfo objects",           /* tp_doc */
    0,0,
    0,0,0,0,               /* tp_iternext */
    MarkupInfo_methods,             /* tp_methods */
    0,
    MarkupInfo_getset,      /*tp_getset */
    0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)MarkupInfo_init,      /* tp_init */
    0,                         /* tp_alloc */
    MarkupInfo_new,                 /* tp_new */
};
PyTypeObject *getMarkupInfoType()
{
    return &MarkupInfoType_;
}
