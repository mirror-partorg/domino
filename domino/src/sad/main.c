#include <Python.h>


static int py_add_path()
{
    PyObject *pModule, *pPath, *pAppend;
    pModule = PyImport_ImportModule("sys");
    if ( !pModule ) {
        return 0;
    }
    pPath   = PyObject_GetAttrString(pModule, "path");
    pAppend = PyObject_GetAttrString(pPath, "append");
    PyObject_CallFunction(pAppend, "s", "/mnt/domino/domino/project/apteka/scripts/import/");
    Py_DECREF(pModule);
    Py_DECREF(pPath);
    Py_DECREF(pAppend);
    return 1;
}

int main(int argc, char *argv[])
{

    // Initialize the Python Interpreter
    Py_Initialize();
    //init_pygobject();
    if ( !py_add_path() )
        exit(1);
    PyObject *pModule, *pDict, *pFunc;
    pModule = PyImport_ImportModule("handbook");
    if ( pModule ) {
        pDict = PyModule_GetDict(pModule);
        if(pDict) {
            fprintf(stderr,"True\n");
            pFunc = PyDict_GetItemString(pDict, "test");
            PyObject_CallFunction(pFunc, "");
        }
        else {
            PyErr_Print();
        }
     Py_DECREF(pModule);
  }
  else {
	PyErr_Print();
	fprintf(stderr,"No Module\n");
  }

    Py_Finalize();

    return 0;
}
