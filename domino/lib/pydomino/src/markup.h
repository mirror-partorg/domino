
#ifndef Markup_H_INCLUDED
#define Markup_H_INCLUDED

#include <Python.h>
#include <domino.h>
#include "../var/alias.h"

typedef struct _MarkupPrivate MarkupPrivate;

typedef struct {
    PyObject_HEAD
    MarkupPrivate *priv;
} Markup;
PyTypeObject *getMarkupType();

#endif // Markup_H_INCLUDED
