
#ifndef Protocol_H_INCLUDED
#define Protocol_H_INCLUDED

#include <Python.h>
#include <domino.h>
#include "alias.h"

typedef struct {
    PyObject_HEAD
    protocol_rec_t *priv;
    Alias *alias;
} Protocol;
PyTypeObject *getProtocolType();

typedef struct {
    PyObject_HEAD
    protocol_key0_t *priv;
    Alias *alias;
} ProtocolKey0;
PyTypeObject *getProtocolKey0Type();

typedef struct {
    PyObject_HEAD
    protocol_key1_t *priv;
    Alias *alias;
} ProtocolKey1;
PyTypeObject *getProtocolKey1Type();

typedef struct {
    PyObject_HEAD
    protocol_key2_t *priv;
    Alias *alias;
} ProtocolKey2;
PyTypeObject *getProtocolKey2Type();

#endif // Protocol_H_INCLUDED
