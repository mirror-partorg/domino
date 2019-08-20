
#ifndef MarkupInfo_H_INCLUDED
#define MarkupInfo_H_INCLUDED

#include <Python.h>
#include <domino.h>
#include "../var/alias.h"

typedef struct {
    PyObject_HEAD
    markup_info_t *info;
} MarkupInfo;
PyTypeObject *getMarkupInfoType();

#endif // MarkupInfo_H_INCLUDED
