
#ifndef ProductLink_H_INCLUDED
#define ProductLink_H_INCLUDED

#include <Python.h>
#include <domino.h>
#include "alias.h"

typedef struct {
    PyObject_HEAD
    product_link_rec_t *priv;
    Alias *alias;
} ProductLink;
PyTypeObject *getProductLinkType();

typedef struct {
    PyObject_HEAD
    product_link_key0_t *priv;
    Alias *alias;
} ProductLinkKey0;
PyTypeObject *getProductLinkKey0Type();

typedef struct {
    PyObject_HEAD
    product_link_key1_t *priv;
    Alias *alias;
} ProductLinkKey1;
PyTypeObject *getProductLinkKey1Type();

#endif // ProductLink_H_INCLUDED
