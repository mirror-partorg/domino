
#ifndef Partner_H_INCLUDED
#define Partner_H_INCLUDED

#include <Python.h>
#include <domino.h>
#include "alias.h"

typedef struct {
    PyObject_HEAD
    partner_rec_t *priv;
    Alias *alias;
} Partner;
PyTypeObject *getPartnerType();

typedef struct {
    PyObject_HEAD
    partner_key0_t *priv;
    Alias *alias;
} PartnerKey0;
PyTypeObject *getPartnerKey0Type();

typedef struct {
    PyObject_HEAD
    partner_key1_t *priv;
    Alias *alias;
} PartnerKey1;
PyTypeObject *getPartnerKey1Type();

typedef struct {
    PyObject_HEAD
    partner_key2_t *priv;
    Alias *alias;
} PartnerKey2;
PyTypeObject *getPartnerKey2Type();

typedef struct {
    PyObject_HEAD
    partner_key3_t *priv;
    Alias *alias;
} PartnerKey3;
PyTypeObject *getPartnerKey3Type();

#endif // Partner_H_INCLUDED
