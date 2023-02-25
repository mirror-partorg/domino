
#ifndef Region_H_INCLUDED
#define Region_H_INCLUDED

#include <Python.h>
#include <domino.h>
#include "alias.h"

typedef struct {
    PyObject_HEAD
    region_rec_t *priv;
    Alias *alias;
} Region;
PyTypeObject *getRegionType();

typedef struct {
    PyObject_HEAD
    region_key0_t *priv;
    Alias *alias;
} RegionKey0;
PyTypeObject *getRegionKey0Type();

typedef struct {
    PyObject_HEAD
    region_key1_t *priv;
    Alias *alias;
} RegionKey1;
PyTypeObject *getRegionKey1Type();

#endif // Region_H_INCLUDED
