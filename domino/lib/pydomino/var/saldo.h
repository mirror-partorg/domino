
#ifndef Saldo_H_INCLUDED
#define Saldo_H_INCLUDED

#include <Python.h>
#include <domino.h>
#include "alias.h"

typedef struct {
    PyObject_HEAD
    saldo_rec_t *priv;
    Alias *alias;
} Saldo;
PyTypeObject *getSaldoType();

typedef struct {
    PyObject_HEAD
    saldo_key0_t *priv;
    Alias *alias;
} SaldoKey0;
PyTypeObject *getSaldoKey0Type();

#endif // Saldo_H_INCLUDED
