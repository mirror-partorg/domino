
#ifndef Grls_H_INCLUDED
#define Grls_H_INCLUDED

#include <Python.h>
#include <domino.h>
#include "../var/alias.h"

typedef struct _GrlsPrivate GrlsPrivate;

typedef struct {
    PyObject_HEAD
    GrlsPrivate *priv;
} Grls;
PyTypeObject *getGrlsType();

#endif // Grls_H_INCLUDED
