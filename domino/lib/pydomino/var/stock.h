
#ifndef Stock_H_INCLUDED
#define Stock_H_INCLUDED

#include <Python.h>
#include <domino.h>
#include "alias.h"

typedef struct {
    PyObject_HEAD
    stock_rec_t *priv;
    Alias *alias;
} Stock;
PyTypeObject *getStockType();

typedef struct {
    PyObject_HEAD
    stock_key0_t *priv;
    Alias *alias;
} StockKey0;
PyTypeObject *getStockKey0Type();

typedef struct {
    PyObject_HEAD
    stock_key1_t *priv;
    Alias *alias;
} StockKey1;
PyTypeObject *getStockKey1Type();

#endif // Stock_H_INCLUDED
