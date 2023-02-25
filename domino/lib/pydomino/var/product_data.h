
#ifndef ProductData_H_INCLUDED
#define ProductData_H_INCLUDED

#include <Python.h>
#include <domino.h>
#include "alias.h"

typedef struct {
    PyObject_HEAD
    product_data_rec_t *priv;
    Alias *alias;
} ProductData;
PyTypeObject *getProductDataType();

typedef struct {
    PyObject_HEAD
    product_data_key0_t *priv;
    Alias *alias;
} ProductDataKey0;
PyTypeObject *getProductDataKey0Type();

typedef struct {
    PyObject_HEAD
    product_data_key1_t *priv;
    Alias *alias;
} ProductDataKey1;
PyTypeObject *getProductDataKey1Type();

typedef struct {
    PyObject_HEAD
    product_data_key2_t *priv;
    Alias *alias;
} ProductDataKey2;
PyTypeObject *getProductDataKey2Type();

#endif // ProductData_H_INCLUDED
