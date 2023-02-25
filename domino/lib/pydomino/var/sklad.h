
#ifndef Store_H_INCLUDED
#define Store_H_INCLUDED

#include <Python.h>
#include <domino.h>
#include "alias.h"

typedef struct {
    PyObject_HEAD
    sklad_rec_t *priv;
    Alias *alias;
} Store;
PyTypeObject *getStoreType();

typedef struct {
    PyObject_HEAD
    sklad_key0_t *priv;
    Alias *alias;
} StoreKey0;
PyTypeObject *getStoreKey0Type();

#endif // Store_H_INCLUDED
