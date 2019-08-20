
#ifndef MISC_H_INCLUDED
#define MISC_H_INCLUDED
#include <Python.h>
#include <dolib.h>

#ifdef __cplusplus
extern "C"
{
#endif

char *get_product_code(PyObject *product);
char *get_barcode_code(PyObject *barcode);
char *get_store_code(PyObject *store);
char *get_partner_code(PyObject *store, int *code);
do_string_list_t *get_store_code_list(PyObject *stores);


#ifdef __cplusplus
}
#endif

#endif // MISC_H_INCLUDED

