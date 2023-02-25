
#ifndef User_H_INCLUDED
#define User_H_INCLUDED

#include <Python.h>
#include <domino.h>
#include "alias.h"

typedef struct {
    PyObject_HEAD
    user_rec_t *priv;
    Alias *alias;
} User;
PyTypeObject *getUserType();

typedef struct {
    PyObject_HEAD
    user_key0_t *priv;
    Alias *alias;
} UserKey0;
PyTypeObject *getUserKey0Type();

#endif // User_H_INCLUDED
