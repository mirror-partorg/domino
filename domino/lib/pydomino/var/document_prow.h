
#ifndef DocumentProw_H_INCLUDED
#define DocumentProw_H_INCLUDED

#include <Python.h>
#include <domino.h>
#include "alias.h"

typedef struct {
    PyObject_HEAD
    document_prow_rec_t *priv;
    Alias *alias;
} DocumentProw;
PyTypeObject *getDocumentProwType();

typedef struct {
    PyObject_HEAD
    document_prow_key0_t *priv;
    Alias *alias;
} DocumentProwKey0;
PyTypeObject *getDocumentProwKey0Type();

#endif // DocumentProw_H_INCLUDED
