
#ifndef View_H_INCLUDED
#define View_H_INCLUDED

#include <Python.h>
#include <domino.h>
#include "../var/alias.h"

typedef struct _ViewPrivate ViewPrivate;

typedef struct {
    PyObject_HEAD
    ViewPrivate *priv;
} View;
PyTypeObject *getViewType();

#endif // View_H_INCLUDED
