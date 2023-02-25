
#ifndef Document_H_INCLUDED
#define Document_H_INCLUDED

#include <Python.h>
#include <domino.h>
#include "alias.h"

typedef struct {
    PyObject_HEAD
    document_rec_t *priv;
    Alias *alias;
} Document;
PyTypeObject *getDocumentType();

typedef struct {
    PyObject_HEAD
    document_key0_t *priv;
    Alias *alias;
} DocumentKey0;
PyTypeObject *getDocumentKey0Type();

typedef struct {
    PyObject_HEAD
    document_key1_t *priv;
    Alias *alias;
} DocumentKey1;
PyTypeObject *getDocumentKey1Type();

typedef struct {
    PyObject_HEAD
    document_key2_t *priv;
    Alias *alias;
} DocumentKey2;
PyTypeObject *getDocumentKey2Type();

typedef struct {
    PyObject_HEAD
    document_key3_t *priv;
    Alias *alias;
} DocumentKey3;
PyTypeObject *getDocumentKey3Type();

typedef struct {
    PyObject_HEAD
    document_key4_t *priv;
    Alias *alias;
} DocumentKey4;
PyTypeObject *getDocumentKey4Type();

#endif // Document_H_INCLUDED
