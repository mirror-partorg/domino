
#ifndef ProductView_H_INCLUDED
#define ProductView_H_INCLUDED

#include <Python.h>
#include <domino.h>
#include "alias.h"

typedef struct {
    PyObject_HEAD
    product_view_rec_t *priv;
    Alias *alias;
} ProductView;
PyTypeObject *getProductViewType();

typedef struct {
    PyObject_HEAD
    product_view_key0_t *priv;
    Alias *alias;
} ProductViewKey0;
PyTypeObject *getProductViewKey0Type();

typedef struct {
    PyObject_HEAD
    product_view_key1_t *priv;
    Alias *alias;
} ProductViewKey1;
PyTypeObject *getProductViewKey1Type();

#endif // ProductView_H_INCLUDED
