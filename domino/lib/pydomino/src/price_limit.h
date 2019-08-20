
#ifndef PriceLimit_H_INCLUDED
#define PriceLimit_H_INCLUDED

#include <Python.h>
#include <domino.h>
#include "../var/alias.h"

typedef struct {
    PyObject_HEAD
    rz_info_t *info;
} PriceLimit;
PyTypeObject *getPriceLimitType();

#endif // PriceLimit_H_INCLUDED
