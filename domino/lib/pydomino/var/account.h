
#ifndef Account_H_INCLUDED
#define Account_H_INCLUDED

#include <Python.h>
#include <domino.h>
#include "alias.h"

typedef struct {
    PyObject_HEAD
    account_rec_t *priv;
    Alias *alias;
} Account;
PyTypeObject *getAccountType();

typedef struct {
    PyObject_HEAD
    account_key0_t *priv;
    Alias *alias;
} AccountKey0;
PyTypeObject *getAccountKey0Type();

#endif // Account_H_INCLUDED
