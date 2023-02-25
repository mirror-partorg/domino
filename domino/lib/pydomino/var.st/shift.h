
#ifndef Shift_H_INCLUDED
#define Shift_H_INCLUDED

#include <Python.h>
#include <domino.h>
#include "alias.h"

typedef struct {
    PyObject_HEAD
    shift_rec_t *priv;
    Alias *alias;
} Shift;
PyTypeObject *getShiftType();

typedef struct {
    PyObject_HEAD
    shift_key0_t *priv;
    Alias *alias;
} ShiftKey0;
PyTypeObject *getShiftKey0Type();

typedef struct {
    PyObject_HEAD
    shift_key1_t *priv;
    Alias *alias;
} ShiftKey1;
PyTypeObject *getShiftKey1Type();

typedef struct {
    PyObject_HEAD
    shift_key2_t *priv;
    Alias *alias;
} ShiftKey2;
PyTypeObject *getShiftKey2Type();

typedef struct {
    PyObject_HEAD
    shift_key3_t *priv;
    Alias *alias;
} ShiftKey3;
PyTypeObject *getShiftKey3Type();

#endif // Shift_H_INCLUDED
