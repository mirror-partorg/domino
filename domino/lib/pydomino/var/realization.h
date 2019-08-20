
#ifndef Realization_H_INCLUDED
#define Realization_H_INCLUDED

#include <Python.h>
#include <domino.h>
#include "alias.h"

typedef struct {
    PyObject_HEAD
    realization_rec_t *priv;
    Alias *alias;
} Realization;
PyTypeObject *getRealizationType();

typedef struct {
    PyObject_HEAD
    realization_key0_t *priv;
    Alias *alias;
} RealizationKey0;
PyTypeObject *getRealizationKey0Type();

typedef struct {
    PyObject_HEAD
    realization_key1_t *priv;
    Alias *alias;
} RealizationKey1;
PyTypeObject *getRealizationKey1Type();

typedef struct {
    PyObject_HEAD
    realization_key2_t *priv;
    Alias *alias;
} RealizationKey2;
PyTypeObject *getRealizationKey2Type();

typedef struct {
    PyObject_HEAD
    realization_key3_t *priv;
    Alias *alias;
} RealizationKey3;
PyTypeObject *getRealizationKey3Type();

typedef struct {
    PyObject_HEAD
    realization_key4_t *priv;
    Alias *alias;
} RealizationKey4;
PyTypeObject *getRealizationKey4Type();

#endif // Realization_H_INCLUDED
