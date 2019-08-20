
#ifndef Product_H_INCLUDED
#define Product_H_INCLUDED

#include <Python.h>
#include <domino.h>
#include "alias.h"

typedef struct {
    PyObject_HEAD
    product_rec_t *priv;
    Alias *alias;
} Product;
PyTypeObject *getProductType();

typedef struct {
    PyObject_HEAD
    product_key0_t *priv;
    Alias *alias;
} ProductKey0;
PyTypeObject *getProductKey0Type();

typedef struct {
    PyObject_HEAD
    product_key1_t *priv;
    Alias *alias;
} ProductKey1;
PyTypeObject *getProductKey1Type();

typedef struct {
    PyObject_HEAD
    product_key2_t *priv;
    Alias *alias;
} ProductKey2;
PyTypeObject *getProductKey2Type();

typedef struct {
    PyObject_HEAD
    product_key3_t *priv;
    Alias *alias;
} ProductKey3;
PyTypeObject *getProductKey3Type();

typedef struct {
    PyObject_HEAD
    product_key4_t *priv;
    Alias *alias;
} ProductKey4;
PyTypeObject *getProductKey4Type();

#endif // Product_H_INCLUDED
