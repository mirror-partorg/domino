
#ifndef Check_H_INCLUDED
#define Check_H_INCLUDED

#include <Python.h>
#include <domino.h>
#include "alias.h"

typedef struct {
    PyObject_HEAD
    check_rec_t *priv;
    Alias *alias;
} Check;
PyTypeObject *getCheckType();

typedef struct {
    PyObject_HEAD
    check_key0_t *priv;
    Alias *alias;
} CheckKey0;
PyTypeObject *getCheckKey0Type();

typedef struct {
    PyObject_HEAD
    check_key1_t *priv;
    Alias *alias;
} CheckKey1;
PyTypeObject *getCheckKey1Type();

typedef struct {
    PyObject_HEAD
    check_key2_t *priv;
    Alias *alias;
} CheckKey2;
PyTypeObject *getCheckKey2Type();

#endif // Check_H_INCLUDED
