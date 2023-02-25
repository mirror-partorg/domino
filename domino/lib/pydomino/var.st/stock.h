
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

typedef struct {
    PyObject_HEAD
    stock_key2_t *priv;
    Alias *alias;
} StockKey2;
PyTypeObject *getStockKey2Type();

typedef struct {
    PyObject_HEAD
    stock_key3_t *priv;
    Alias *alias;
} StockKey3;
PyTypeObject *getStockKey3Type();

#endif // Stock_H_INCLUDED
