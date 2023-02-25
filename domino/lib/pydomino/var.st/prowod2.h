
#ifndef Prowod2_H_INCLUDED
#define Prowod2_H_INCLUDED

#include <Python.h>
#include <domino.h>
#include "alias.h"

typedef struct {
    PyObject_HEAD
    prowod2_rec_t *priv;
    Alias *alias;
} Prowod2;
PyTypeObject *getProwod2Type();

typedef struct {
    PyObject_HEAD
    prowod2_key0_t *priv;
    Alias *alias;
} Prowod2Key0;
PyTypeObject *getProwod2Key0Type();

typedef struct {
    PyObject_HEAD
    prowod2_key1_t *priv;
    Alias *alias;
} Prowod2Key1;
PyTypeObject *getProwod2Key1Type();

typedef struct {
    PyObject_HEAD
    prowod2_key2_t *priv;
    Alias *alias;
} Prowod2Key2;
PyTypeObject *getProwod2Key2Type();

typedef struct {
    PyObject_HEAD
    prowod2_key3_t *priv;
    Alias *alias;
} Prowod2Key3;
PyTypeObject *getProwod2Key3Type();

#endif // Prowod2_H_INCLUDED
