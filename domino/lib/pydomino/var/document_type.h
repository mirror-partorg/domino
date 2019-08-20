
#ifndef DocumentType_H_INCLUDED
#define DocumentType_H_INCLUDED

#include <Python.h>
#include <domino.h>
#include "alias.h"

typedef struct {
    PyObject_HEAD
    document_type_rec_t *priv;
    Alias *alias;
} DocumentType;
PyTypeObject *getDocumentTypeType();

typedef struct {
    PyObject_HEAD
    document_type_key0_t *priv;
    Alias *alias;
} DocumentTypeKey0;
PyTypeObject *getDocumentTypeKey0Type();

#endif // DocumentType_H_INCLUDED
