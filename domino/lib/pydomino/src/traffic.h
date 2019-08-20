
#ifndef Traffic_H_INCLUDED
#define Traffic_H_INCLUDED

#include <Python.h>
#include <domino.h>
#include "../var/alias.h"

typedef struct _TrafficPrivate TrafficPrivate;

typedef struct {
    PyObject_HEAD
    TrafficPrivate *priv;
} Traffic;
PyTypeObject *getTrafficType();

#endif // Traffic_H_INCLUDED
