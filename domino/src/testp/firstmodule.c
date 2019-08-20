#include <Python.h>

PyMODINIT_FUNC
initfirst(void) 
{
Py_InitModule3("first", NULL,
       "Example module's docstring.");
}