
#ifndef Group_H_INCLUDED
#define Group_H_INCLUDED

#include <Python.h>
#include <domino.h>
#include "alias.h"

typedef struct {
    PyObject_HEAD
    group_rec_t *priv;
    Alias *alias;
} Group;
PyTypeObject *getGroupType();

typedef struct {
    PyObject_HEAD
    group_key0_t *priv;
    Alias *alias;
} GroupKey0;
PyTypeObject *getGroupKey0Type();

typedef struct {
    PyObject_HEAD
    group_key1_t *priv;
    Alias *alias;
} GroupKey1;
PyTypeObject *getGroupKey1Type();

#endif // Group_H_INCLUDED
