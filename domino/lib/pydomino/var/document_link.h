
#ifndef DocumentLink_H_INCLUDED
#define DocumentLink_H_INCLUDED

#include <Python.h>
#include <domino.h>
#include "alias.h"

typedef struct {
    PyObject_HEAD
    document_link_rec_t *priv;
    Alias *alias;
} DocumentLink;
PyTypeObject *getDocumentLinkType();

typedef struct {
    PyObject_HEAD
    document_link_key0_t *priv;
    Alias *alias;
} DocumentLinkKey0;
PyTypeObject *getDocumentLinkKey0Type();

typedef struct {
    PyObject_HEAD
    document_link_key1_t *priv;
    Alias *alias;
} DocumentLinkKey1;
PyTypeObject *getDocumentLinkKey1Type();

#endif // DocumentLink_H_INCLUDED
