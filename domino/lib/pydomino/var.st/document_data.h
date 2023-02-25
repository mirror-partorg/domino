
#ifndef DocumentData_H_INCLUDED
#define DocumentData_H_INCLUDED

#include <Python.h>
#include <domino.h>
#include "alias.h"

typedef struct {
    PyObject_HEAD
    document_data_rec_t *priv;
    Alias *alias;
} DocumentData;
PyTypeObject *getDocumentDataType();

typedef struct {
    PyObject_HEAD
    document_data_key0_t *priv;
    Alias *alias;
} DocumentDataKey0;
PyTypeObject *getDocumentDataKey0Type();

typedef struct {
    PyObject_HEAD
    document_data_key1_t *priv;
    Alias *alias;
} DocumentDataKey1;
PyTypeObject *getDocumentDataKey1Type();

typedef struct {
    PyObject_HEAD
    document_data_key2_t *priv;
    Alias *alias;
} DocumentDataKey2;
PyTypeObject *getDocumentDataKey2Type();

#endif // DocumentData_H_INCLUDED
