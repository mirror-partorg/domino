
#ifndef ALIAS_H_INCLUDED
#define ALIAS_H_INCLUDED

#include <Python.h>
#include <domino.h>

typedef struct {
    PyObject_HEAD
    do_alias_t *alias;
} Alias;
PyTypeObject *getAliasType();


#endif // ALIAS_H_INCLUDED
