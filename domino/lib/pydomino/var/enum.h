
#ifndef Enum_H_INCLUDED
#define Enum_H_INCLUDED

#include <Python.h>
#include <domino.h>
#include "alias.h"

typedef struct {
    PyObject_HEAD
    enum_rec_t *priv;
    Alias *alias;
} Enum;
PyTypeObject *getEnumType();

typedef struct {
    PyObject_HEAD
    enum_key0_t *priv;
    Alias *alias;
} EnumKey0;
PyTypeObject *getEnumKey0Type();

#endif // Enum_H_INCLUDED
