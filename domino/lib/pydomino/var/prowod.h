
#ifndef Prowod_H_INCLUDED
#define Prowod_H_INCLUDED

#include <Python.h>
#include <domino.h>
#include "alias.h"

typedef struct {
    PyObject_HEAD
    prowod_rec_t *priv;
    Alias *alias;
} Prowod;
PyTypeObject *getProwodType();

typedef struct {
    PyObject_HEAD
    prowod_key0_t *priv;
    Alias *alias;
} ProwodKey0;
PyTypeObject *getProwodKey0Type();

typedef struct {
    PyObject_HEAD
    prowod_key1_t *priv;
    Alias *alias;
} ProwodKey1;
PyTypeObject *getProwodKey1Type();

typedef struct {
    PyObject_HEAD
    prowod_key2_t *priv;
    Alias *alias;
} ProwodKey2;
PyTypeObject *getProwodKey2Type();

typedef struct {
    PyObject_HEAD
    prowod_key3_t *priv;
    Alias *alias;
} ProwodKey3;
PyTypeObject *getProwodKey3Type();

typedef struct {
    PyObject_HEAD
    prowod_key4_t *priv;
    Alias *alias;
} ProwodKey4;
PyTypeObject *getProwodKey4Type();

#endif // Prowod_H_INCLUDED
