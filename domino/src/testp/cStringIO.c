#include "Python.h"
#include "structmember.h"

PyDoc_STRVAR(cStringIO_doc,
"A simple fast partial StringIO replacement.\n"
"\n"
"This module provides a simple useful replacement for the StringIO\n"
"module that is written in C.  It does not provide the full\n"
"generality of StringIO, but it provides enough for most\n"
"applications and is especially useful in conjunction with the\n"
"pickle module.\n"
"\n"
"Usage:\n"
"\n"
"from cStringIO import StringIO\n"
"\n"
"an_output_stream = StringIO()\n"
"an_output_stream.write(some_stuff)\n"
"...\n"
"value = an_output_stream.getvalue()\n"
"\n"
"an_input_stream = StringIO(a_string)\n"
"spam = an_input_stream.readline()\n"
"spam = an_input_stream.read(5)\n"
"an_input_stream.seek(0)             # OK, start over\n"
"spam = an_input_stream.read()       # and read it all\n"
"\n");

typedef struct {
        PyObject_HEAD
        char *buf;
        int pos;
        int size;
        PyObject *pbuf;
} InputObject;

PyDoc_STRVAR(input_doc,
"Simple type for treating strings as input file streams\n");

static PyObject *
input_close(PyObject *self)
{
}

PyDoc_STRVAR(input_flush_doc,
"Does nothing\n");

static PyObject *
input_flush(PyObject *self)
{
}

PyDoc_STRVAR(input_getvalue_doc,
"Get the string value.\n"
"\n"
"If use_pos is specified and is a true value, then the\n"
"string returned will include only the text up to the\n"
"current file position.\n"
"\n");

static PyObject *
input_getvalue(PyObject *self)
{
}

PyDoc_STRVAR(input_isatty_doc,
"Always returns False\n");

static PyObject *
input_isatty(PyObject *self)
{
}

PyDoc_STRVAR(input_read_doc,
"Return s characters or the rest of the string.\n");

static PyObject *
input_read(PyObject *self, PyObject *args)
{
        int s;
        if (!PyArg_ParseTuple(args, "|i:read",
                              &s))
                return NULL;
}

PyDoc_STRVAR(input_readline_doc,
"Read one line.\n");

static PyObject *
input_readline(PyObject *self)
{
}

PyDoc_STRVAR(input_readlines_doc,
"Read all lines.\n");

static PyObject *
input_readlines(PyObject *self, PyObject *args)
{
        int hint;
        if (!PyArg_ParseTuple(args, "|i:readlines",
                              &hint))
                return NULL;
}

PyDoc_STRVAR(input_reset_doc,
"Reset the file position to the beginning.\n");

static PyObject *
input_reset(PyObject *self)
{
}

PyDoc_STRVAR(input_seek_doc,
"Set the current position.\n"
"\n"
"The optional mode argument can be 0 for absolute, 1 for relative,\n"
"and 2 for relative to EOF.  The default is absolute.\n"
"\n");

static PyObject *
input_seek(PyObject *self, PyObject *args)
{
        int position;
        int mode = 0;
        if (!PyArg_ParseTuple(args, "i|i:seek",
                              &position, &mode))
                return NULL;
}

PyDoc_STRVAR(input_tell_doc,
"Get the current position.\n");

static PyObject *
input_tell(PyObject *self)
{
}

PyDoc_STRVAR(input_truncate_doc,
"Truncate the file at the current position.\n");

static PyObject *
input_truncate(PyObject *self, PyObject *args)
{
        int pos;
        if (!PyArg_ParseTuple(args, "|i:truncate",
                              &pos))
                return NULL;
}

static struct PyMethodDef input_methods[] = {
        {"close", (PyCFunction)input_close, METH_NOARGS},
        {"flush", (PyCFunction)input_flush, METH_NOARGS,
         input_flush_doc},
        {"getvalue", (PyCFunction)input_getvalue, METH_NOARGS,
         input_getvalue_doc},
        {"isatty", (PyCFunction)input_isatty, METH_NOARGS,
         input_isatty_doc},
        {"read", (PyCFunction)input_read, METH_VARARGS,
         input_read_doc},
        {"readline", (PyCFunction)input_readline, METH_NOARGS,
         input_readline_doc},
        {"readlines", (PyCFunction)input_readlines, METH_VARARGS,
         input_readlines_doc},
        {"reset", (PyCFunction)input_reset, METH_NOARGS,
         input_reset_doc},
        {"seek", (PyCFunction)input_seek, METH_VARARGS,
         input_seek_doc},
        {"tell", (PyCFunction)input_tell, METH_NOARGS,
         input_tell_doc},
        {"truncate", (PyCFunction)input_truncate, METH_VARARGS,
         input_truncate_doc},
        {NULL, NULL}
};

static void
input_dealloc(PyObject *ob)
{
}

static PyTypeObject PyInputType_Type = {
        PyObject_HEAD_INIT(0)
        0,				/* ob_size */
        "cStringIO.InputType",	/* tp_name */
        sizeof(InputObject),		/* tp_basicsize */
        0,				/* tp_itemsize */
        input_dealloc,		/* tp_dealloc */
        0,				/* tp_print */
        0,				/* tp_getattr */
        0,				/* tp_setattr */
        0,				/* tp_compare */
        0,				/* tp_repr */
        0,				/* tp_as_number */
        0,				/* tp_as_sequence */
        0,				/* tp_as_mapping */
        0,				/* tp_hash */
        0,				/* tp_call */
        0,				/* tp_str */
        PyObject_GenericGetAttr,	/* tp_getattro */
        0,				/* tp_setattro */
        0,				/* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,		/* tp_flags */
        input_doc,			/* tp_doc */
        0,				/* tp_traverse */
        0,				/* tp_clear */
        0,				/* tp_richcompare */
        0,				/* tp_weaklistoffset */
        0,				/* tp_iter */
        0,				/* tp_iternext */
        input_methods,		/* tp_methods */
        0,				/* tp_members */
        0,				/* tp_getset */
        0,				/* tp_base */
        0,				/* tp_dict */
        0,				/* tp_descr_get */
        0,				/* tp_descr_set */
        0,				/* tp_dictoffset */
        0,				/* tp_init */
        0,				/* tp_alloc */
        0,				/* tp_new */
        0,				/* tp_free */
        0,				/* tp_is_gc */
};

typedef struct {
        PyObject_HEAD
        char *buf;
        int pos;
        int size;
        int softspace;
} OutputObject;

PyDoc_STRVAR(output_doc,
"Simple type for output strings.\n");

static PyObject *
output_close(PyObject *self)
{
}

PyDoc_STRVAR(output_flush_doc,
"Does nothing\n");

static PyObject *
output_flush(PyObject *self)
{
}

PyDoc_STRVAR(output_getvalue_doc,
"Get the string value.\n"
"\n"
"If use_pos is specified and is a true value, then the\n"
"string returned will include only the text up to the\n"
"current file position.\n"
"\n");

static PyObject *
output_getvalue(PyObject *self)
{
}

PyDoc_STRVAR(output_isatty_doc,
"Always returns False\n");

static PyObject *
output_isatty(PyObject *self)
{
}

PyDoc_STRVAR(output_read_doc,
"Return s characters or the rest of the string.\n");

static PyObject *
output_read(PyObject *self, PyObject *args)
{
        int s;
        if (!PyArg_ParseTuple(args, "|i:read",
                              &s))
                return NULL;
}

PyDoc_STRVAR(output_readline_doc,
"Read one line.\n");

static PyObject *
output_readline(PyObject *self)
{
}

PyDoc_STRVAR(output_readlines_doc,
"Read all lines.\n");

static PyObject *
output_readlines(PyObject *self, PyObject *args)
{
        int hint;
        if (!PyArg_ParseTuple(args, "|i:readlines",
                              &hint))
                return NULL;
}

PyDoc_STRVAR(output_reset_doc,
"Reset the file position to the beginning.\n");

static PyObject *
output_reset(PyObject *self)
{
}

PyDoc_STRVAR(output_seek_doc,
"Set the current position.\n"
"\n"
"The optional mode argument can be 0 for absolute, 1 for relative,\n"
"and 2 for relative to EOF.  The default is absolute.\n"
"\n");

static PyObject *
output_seek(PyObject *self, PyObject *args)
{
        int position;
        int mode = 0;
        if (!PyArg_ParseTuple(args, "i|i:seek",
                              &position, &mode))
                return NULL;
}

PyDoc_STRVAR(output_tell_doc,
"Get the current position.\n");

static PyObject *
output_tell(PyObject *self)
{
}

PyDoc_STRVAR(output_truncate_doc,
"Truncate the file at the current position.\n");

static PyObject *
output_truncate(PyObject *self, PyObject *args)
{
        int pos;
        if (!PyArg_ParseTuple(args, "|i:truncate",
                              &pos))
                return NULL;
}

PyDoc_STRVAR(output_write_doc,
"Write a string to the file.\n");

static PyObject *
output_write(PyObject *self, PyObject *s)
{
}

PyDoc_STRVAR(output_writelines_doc,
"Write each string in lines.\n");

static PyObject *
output_writelines(PyObject *self, PyObject *lines)
{
}

static struct PyMethodDef output_methods[] = {
        {"close", (PyCFunction)output_close, METH_NOARGS},
        {"flush", (PyCFunction)output_flush, METH_NOARGS,
         output_flush_doc},
        {"getvalue", (PyCFunction)output_getvalue, METH_NOARGS,
         output_getvalue_doc},
        {"isatty", (PyCFunction)output_isatty, METH_NOARGS,
         output_isatty_doc},
        {"read", (PyCFunction)output_read, METH_VARARGS,
         output_read_doc},
        {"readline", (PyCFunction)output_readline, METH_NOARGS,
         output_readline_doc},
        {"readlines", (PyCFunction)output_readlines, METH_VARARGS,
         output_readlines_doc},
        {"reset", (PyCFunction)output_reset, METH_NOARGS,
         output_reset_doc},
        {"seek", (PyCFunction)output_seek, METH_VARARGS,
         output_seek_doc},
        {"tell", (PyCFunction)output_tell, METH_NOARGS,
         output_tell_doc},
        {"truncate", (PyCFunction)output_truncate, METH_VARARGS,
         output_truncate_doc},
        {"write", (PyCFunction)output_write, METH_O,
         output_write_doc},
        {"writelines", (PyCFunction)output_writelines, METH_O,
         output_writelines_doc},
        {NULL, NULL}
};

#define OFF(X) offsetof(OutputObject, X)

static struct PyMemberDef output_members[] = {
        {"softspace", T_INT, OFF(softspace), 0},
        {NULL}
};

#undef OFF

static void
output_dealloc(PyObject *ob)
{
}

static PyTypeObject PyOutputType_Type = {
        PyObject_HEAD_INIT(0)
        0,				/* ob_size */
        "cStringIO.OutputType",	/* tp_name */
        sizeof(OutputObject),	/* tp_basicsize */
        0,				/* tp_itemsize */
        output_dealloc,		/* tp_dealloc */
        0,				/* tp_print */
        0,				/* tp_getattr */
        0,				/* tp_setattr */
        0,				/* tp_compare */
        0,				/* tp_repr */
        0,				/* tp_as_number */
        0,				/* tp_as_sequence */
        0,				/* tp_as_mapping */
        0,				/* tp_hash */
        0,				/* tp_call */
        0,				/* tp_str */
        PyObject_GenericGetAttr,	/* tp_getattro */
        0,				/* tp_setattro */
        0,				/* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,		/* tp_flags */
        output_doc,			/* tp_doc */
        0,				/* tp_traverse */
        0,				/* tp_clear */
        0,				/* tp_richcompare */
        0,				/* tp_weaklistoffset */
        0,				/* tp_iter */
        0,				/* tp_iternext */
        output_methods,		/* tp_methods */
        output_members,		/* tp_members */
        0,				/* tp_getset */
        0,				/* tp_base */
        0,				/* tp_dict */
        0,				/* tp_descr_get */
        0,				/* tp_descr_set */
        0,				/* tp_dictoffset */
        0,				/* tp_init */
        0,				/* tp_alloc */
        0,				/* tp_new */
        0,				/* tp_free */
        0,				/* tp_is_gc */
};

PyDoc_STRVAR(cStringIO_StringIO_doc,
"Return a StringIO-like stream for reading or writing\n");

static PyObject *
cStringIO_StringIO(PyObject *self, PyObject *args)
{
        PyObject *o;
        if (!PyArg_ParseTuple(args, "|O:StringIO",
                              &o))
                return NULL;
}

static struct PyMethodDef cStringIO_methods[] = {
        {"StringIO", (PyCFunction)cStringIO_StringIO, METH_VARARGS,
         cStringIO_StringIO_doc},
        {NULL, NULL}
};

PyMODINIT_FUNC
initcStringIO(void)
{
        PyObject *mod;

        mod = Py_InitModule3("cStringIO", cStringIO_methods,
                             cStringIO_doc);
        if (mod == NULL)
                return;

        PyInputType_Type.ob_type = &PyType_Type;
        if (!PyObject_SetAttrString(mod, "InputType",
                                    (PyObject *)&PyInputType_Type))
                return;

        PyOutputType_Type.ob_type = &PyType_Type;
        if (!PyObject_SetAttrString(mod, "OutputType",
                                    (PyObject *)&PyOutputType_Type))
                return;

}
