
#ifndef Class_H_INCLUDED
#define Class_H_INCLUDED

#include <Python.h>
#include <domino.h>
#include "alias.h"

typedef struct {
    PyObject_HEAD
    class_rec_t *priv;
    Alias *alias;
} Class;
PyTypeObject *getClassType();

typedef struct {
    PyObject_HEAD
    class_key0_t *priv;
    Alias *alias;
} ClassKey0;
PyTypeObject *getClassKey0Type();

#endif // Class_H_INCLUDED
