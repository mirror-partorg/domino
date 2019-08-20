
#ifndef DocumentOrder_H_INCLUDED
#define DocumentOrder_H_INCLUDED

#include <Python.h>
#include <domino.h>
#include "alias.h"

typedef struct {
    PyObject_HEAD
    document_order_rec_t *priv;
    Alias *alias;
} DocumentOrder;
PyTypeObject *getDocumentOrderType();

typedef struct {
    PyObject_HEAD
    document_order_key0_t *priv;
    Alias *alias;
} DocumentOrderKey0;
PyTypeObject *getDocumentOrderKey0Type();

typedef struct {
    PyObject_HEAD
    document_order_key1_t *priv;
    Alias *alias;
} DocumentOrderKey1;
PyTypeObject *getDocumentOrderKey1Type();

typedef struct {
    PyObject_HEAD
    document_order_key2_t *priv;
    Alias *alias;
} DocumentOrderKey2;
PyTypeObject *getDocumentOrderKey2Type();

typedef struct {
    PyObject_HEAD
    document_order_key3_t *priv;
    Alias *alias;
} DocumentOrderKey3;
PyTypeObject *getDocumentOrderKey3Type();

typedef struct {
    PyObject_HEAD
    document_order_key4_t *priv;
    Alias *alias;
} DocumentOrderKey4;
PyTypeObject *getDocumentOrderKey4Type();

#endif // DocumentOrder_H_INCLUDED
