
#include <Python.h>
#include "testp.h"


typedef struct pytestpobject {
    PyObject_HEAD
    testp_t *testp;
} pytestpobject;


static PyTypeObject PyTestp;

#define is_pytestobject(v) ((v)->ob_type == &PyTestp)

static pytestpobject *pytestpobject_new(double radius)
{
    pytestpobject *pytest;
    pytest = PyObject_New(pytestpobject, &PyTestp);
    if ( !pytest )
        return NULL;
    pytest->testp = testp_new(radius);
    return pytest;
}
static void pytestpobject_free(pytestpobject *pytest)
{
    fprintf(stderr, "!!DEAD\n");
    testp_free(pytest->testp);
    PyObject_Del(pytest);
}
static PyObject *pytestpobject_get_area(pytestpobject *pytestp)//, PyObject *args)
{

    /*if ( !PyArg_ParseTuple(args, "") )
        return NULL;*/
    double retval;
    retval = testp_get_area(pytestp->testp);

    return Py_BuildValue("d", retval);
}

// Строка документации метода update
PyDoc_STRVAR(get_area,
"get_area () -> double\n\
\n\
get area.");

static PyMethodDef pytestpobject_methods[] = {

{"get_area",  (PyCFunction)pytestpobject_get_area,    METH_NOARGS, get_area},
{NULL, NULL}                             /* sentinel */
};

static PyObject *
pytestpobject_getattr(pytestpobject *self, char *name)
{
        // атрибут-данное digest_size
      /*  if (strcmp(name, "digest_size") == 0) {
                return PyInt_FromLong(16);
        }*/
        // поиск атрибута-метода ведется в списке
        return Py_FindMethod(pytestpobject_methods, (PyObject *)self, name);
}

PyDoc_STRVAR(module_doc, "This module is test ...");

// Строка документации к классу md5
PyDoc_STRVAR(pytestpobjecttype_doc, "An test object...");

static PyTypeObject PyTestptype = {
        PyObject_HEAD_INIT(NULL)
        0,                    /*ob_size*/
        "testp.Sad",            /*tp_name*/
        sizeof(pytestpobject),    /*tp_size*/
        0,                    /*tp_itemsize*/
        /* methods */
        (destructor)pytestpobject_free,  /*tp_dealloc*/
        0,                    /*tp_print*/
        (getattrfunc)pytestpobject_getattr, /*tp_getattr*/
        0,                    /*tp_setattr*/
        0,                    /*tp_compare*/
        0,                    /*tp_repr*/
        0,                    /*tp_as_number*/
        0,                        /*tp_as_sequence*/
        0,                    /*tp_as_mapping*/
        0,                    /*tp_hash*/
        0,                    /*tp_call*/
        0,                    /*tp_str*/
        0,                    /*tp_getattro*/
        0,                    /*tp_setattro*/
        0,                    /*tp_as_buffer*/
        0,                    /*tp_xxx4*/
        pytestpobjecttype_doc,          /*tp_doc*/
};


static PyObject *PyTestp_new(PyObject *self, PyObject *args)
{
    pytestpobject *retval;
    double radius;

    if ( !PyArg_ParseTuple(args, "d:radius", &radius) )
        return NULL;

    /*if ( (retval = pytestpobject_new(radius)) == NULL )
        return NULL;*/

    fprintf(stderr,"!!!1 %d %f\n", retval, radius);

    //return (PyObject *)retval;
}

// Строка документации для new()
PyDoc_STRVAR(new_doc, "new([arg]) -> testp object ...");

// Список функций, которые данный модуль экспортирует
static PyMethodDef testp_functions[] = {
        {"new",             (PyCFunction)PyTestp_new, METH_VARARGS, new_doc},
        {"testp",           (PyCFunction)PyTestp_new, METH_VARARGS, new_doc},
        {NULL,              NULL}   /* Sentinel */
};
// Следует заметить, что md5 - то же самое, что new. Эта функция оставлена для
// обратной совместимости со старым модулем md5

// Инициализация модуля
PyMODINIT_FUNC
inittestp(void)
{
        PyObject *m, *d;

        // Инициализируется модуль
        m = Py_InitModule3("testp", testp_functions, module_doc);

        // Получается словарь с именами модуля
        d = PyModule_GetDict(m);

        /*PyOutputType_Type.ob_type = &PyType_Type;
        if (!PyObject_SetAttrString(mod, "OutputType",
                                    (PyObject *)&PyOutputType_Type))
                return;*/

        // Добавляется атрибут MD5Type (тип md5-объекта) к словарю
        PyTestptype.ob_type = &PyType_Type;
        PyObject_SetAttrString(m, "PyTestpType", (PyObject *)&PyTestptype);
        //PyDict_SetItemString(m, "PyTestpType", (PyObject *)&PyTestptype);
        // Добавляется целая константа digest_size к модулю
        //PyModule_AddIntConstant(m, "digest_size", 16);
}
