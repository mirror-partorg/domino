
#ifndef CheckSum_H_INCLUDED
#define CheckSum_H_INCLUDED

#include <Python.h>
#include <domino.h>
#include "alias.h"

typedef struct {
    PyObject_HEAD
    checksum_rec_t *priv;
    Alias *alias;
} CheckSum;
PyTypeObject *getCheckSumType();

typedef struct {
    PyObject_HEAD
    checksum_key0_t *priv;
    Alias *alias;
} CheckSumKey0;
PyTypeObject *getCheckSumKey0Type();

typedef struct {
    PyObject_HEAD
    checksum_key1_t *priv;
    Alias *alias;
} CheckSumKey1;
PyTypeObject *getCheckSumKey1Type();

#endif // CheckSum_H_INCLUDED
