
#ifndef DocumentView_H_INCLUDED
#define DocumentView_H_INCLUDED

#include <Python.h>
#include <domino.h>
#include "alias.h"

typedef struct {
    PyObject_HEAD
    document_view_rec_t *priv;
    Alias *alias;
} DocumentView;
PyTypeObject *getDocumentViewType();

typedef struct {
    PyObject_HEAD
    document_view_key0_t *priv;
    Alias *alias;
} DocumentViewKey0;
PyTypeObject *getDocumentViewKey0Type();

typedef struct {
    PyObject_HEAD
    document_view_key1_t *priv;
    Alias *alias;
} DocumentViewKey1;
PyTypeObject *getDocumentViewKey1Type();

#endif // DocumentView_H_INCLUDED
