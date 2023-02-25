
#ifndef PartnerData_H_INCLUDED
#define PartnerData_H_INCLUDED

#include <Python.h>
#include <domino.h>
#include "alias.h"

typedef struct {
    PyObject_HEAD
    partner_data_rec_t *priv;
    Alias *alias;
} PartnerData;
PyTypeObject *getPartnerDataType();

typedef struct {
    PyObject_HEAD
    partner_data_key0_t *priv;
    Alias *alias;
} PartnerDataKey0;
PyTypeObject *getPartnerDataKey0Type();

typedef struct {
    PyObject_HEAD
    partner_data_key1_t *priv;
    Alias *alias;
} PartnerDataKey1;
PyTypeObject *getPartnerDataKey1Type();

typedef struct {
    PyObject_HEAD
    partner_data_key2_t *priv;
    Alias *alias;
} PartnerDataKey2;
PyTypeObject *getPartnerDataKey2Type();

#endif // PartnerData_H_INCLUDED
