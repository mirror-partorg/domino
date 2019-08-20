
#ifndef Subgroup_H_INCLUDED
#define Subgroup_H_INCLUDED

#include <Python.h>
#include <domino.h>
#include "alias.h"

typedef struct {
    PyObject_HEAD
    subgroup_rec_t *priv;
    Alias *alias;
} Subgroup;
PyTypeObject *getSubgroupType();

typedef struct {
    PyObject_HEAD
    subgroup_key0_t *priv;
    Alias *alias;
} SubgroupKey0;
PyTypeObject *getSubgroupKey0Type();

typedef struct {
    PyObject_HEAD
    subgroup_key1_t *priv;
    Alias *alias;
} SubgroupKey1;
PyTypeObject *getSubgroupKey1Type();

#endif // Subgroup_H_INCLUDED
