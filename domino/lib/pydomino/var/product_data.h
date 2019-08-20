
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

#endif // ProductData_H_INCLUDED
