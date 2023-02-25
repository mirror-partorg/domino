
#ifndef Division_H_INCLUDED
#define Division_H_INCLUDED

#include <Python.h>
#include <domino.h>
#include "alias.h"

typedef struct {
    PyObject_HEAD
    otdel_rec_t *priv;
    Alias *alias;
} Division;
PyTypeObject *getDivisionType();

typedef struct {
    PyObject_HEAD
    otdel_key0_t *priv;
    Alias *alias;
} DivisionKey0;
PyTypeObject *getDivisionKey0Type();

#endif // Division_H_INCLUDED
