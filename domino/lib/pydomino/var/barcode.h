
#ifndef Barcode_H_INCLUDED
#define Barcode_H_INCLUDED

#include <Python.h>
#include <domino.h>
#include "alias.h"

typedef struct {
    PyObject_HEAD
    barcode_rec_t *priv;
    Alias *alias;
} Barcode;
PyTypeObject *getBarcodeType();

typedef struct {
    PyObject_HEAD
    barcode_key0_t *priv;
    Alias *alias;
} BarcodeKey0;
PyTypeObject *getBarcodeKey0Type();

typedef struct {
    PyObject_HEAD
    barcode_key1_t *priv;
    Alias *alias;
} BarcodeKey1;
PyTypeObject *getBarcodeKey1Type();

#endif // Barcode_H_INCLUDED
