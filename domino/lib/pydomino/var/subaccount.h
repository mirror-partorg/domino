
#ifndef Subaccount_H_INCLUDED
#define Subaccount_H_INCLUDED

#include <Python.h>
#include <domino.h>
#include "alias.h"

typedef struct {
    PyObject_HEAD
    subaccount_rec_t *priv;
    Alias *alias;
} Subaccount;
PyTypeObject *getSubaccountType();

typedef struct {
    PyObject_HEAD
    subaccount_key0_t *priv;
    Alias *alias;
} SubaccountKey0;
PyTypeObject *getSubaccountKey0Type();

#endif // Subaccount_H_INCLUDED
