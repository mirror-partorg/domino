


#include <Python.h>
#include "util.h"
#include <dolib.h>
#include <datetime.h>

static PyObject *Error = NULL;
PyObject *getDominoErrorObject()
{
    if ( !Error ) {
        Error = PyErr_NewException("pydomino.error", NULL, NULL);
        Py_INCREF(Error);
    }

    return Error;
}
PyObject* do_getparams(const char* src)
{
    PyObject *result;
    PyObject *val;
    result = PyDict_New();

    char *cp = (char*)src, *head;
    char name[1024];
    char value[4098];
    while (*cp != '\0') {
        head = cp;
        while (*cp != '\0' && *cp != '\n' && *cp !='\r' && *cp != '=') cp++;
        if (*cp != '=' ) {
            while (*cp == '\n' || *cp =='\r') cp++;
            continue;
        }
        strncpy(name, head, cp - head);
        name[cp - head] = '\0';
        head = ++cp;
        while (*cp != '\0' && *cp != '\n' && *cp !='\r') cp++;
        strncpy(value, head, cp - head);
        value[cp - head] = '\0';
        if ( strlen(name) > 0 ) {
            val = MyString_FromString(value);
            PyDict_SetItemString(result, name, val);
        }
        while (*cp == '\n' || *cp =='\r') cp++;
    }
    return result;
}
char *get_desc(const char *name, PyObject *value)
{
    char *buf;
    PyDateTime_IMPORT;    
    if ( MyString_Check(value) ) {
        char *val;
        val = MyString_AsString(value);
        buf = do_strdup_printf("%s:\"%s\"", name, val);
        do_free(val);
    }
    else
    if ( MyLong_Check(value) )
        buf = do_strdup_printf("%s:%ld", name, MyLong_AsLong(value));
    else
    if ( PyFloat_Check(value) )
        buf = do_strdup_printf("%s:%f", name, PyFloat_AsDouble(value));
    else
    if ( PyDate_Check(value) )
        buf = do_strdup_printf("%s:%2.2d/%2.2d/%d", name,  PyDateTime_GET_DAY(value), PyDateTime_GET_MONTH(value), PyDateTime_GET_YEAR(value));
    else
    if ( PyTime_Check(value) ) {
        buf = do_strdup_printf("%s:%2.2d:%2.2d:%2.2d", name, PyDateTime_TIME_GET_HOUR(value), PyDateTime_TIME_GET_MINUTE(value), PyDateTime_TIME_GET_SECOND(value));
    }
    else
    if ( PyDateTime_Check(value) )
        buf = do_strdup_printf("%s:%2.2d/%2.2d/%d %2.2d %2.2d %2.2d", name,  PyDateTime_GET_DAY(value), PyDateTime_GET_MONTH(value), PyDateTime_GET_YEAR(value), PyDateTime_DATE_GET_HOUR(value), PyDateTime_DATE_GET_MINUTE(value), PyDateTime_DATE_GET_SECOND(value));
    else {
        do_log(LOG_ERR, "Invalid type field \"%s\"", name);
        buf = do_strdup_printf("%s:unknow", name);
    }
    return buf;
}

char *get_prices_desc(PyObject *value) 
{
    char *buf;
    int i;
    PyObject *pKeys = PyDict_Keys(value);
    buf = NULL;
    for ( i = 0; i < PyList_Size(pKeys); ++i ) {
        char *v,*v1;
        PyObject *pKey = PyList_GetItem(pKeys, i);
        PyObject *pValue;
        pValue = PyDict_GetItem(value, pKey);
        v = do_strdup_printf("%ld:%f",  MyLong_AsLong(pKey), PyFloat_AsDouble(pValue));
        if ( buf ) {
            v1 = do_strdup_printf("%s,%s", buf, v);
            do_free(buf);do_free(v);
            buf = v1;
        }
        else 
            buf = v;
    }
    if ( buf ) {
       char *v;
       v = do_strdup_printf("Prices:[%s]", buf);
       do_free(buf);
       buf = v;
    }
    else
       buf=do_strdup("Prices:[]");
    return buf;
}
char *get_quants_desc(PyObject *value, const char *name) 
{
    char *buf;
    PyObject *pKeys = PyDict_Keys(value);
    int i;
    buf = NULL;
    for ( i = 0; i < PyList_Size(pKeys); ++i ) {
        char *v,*v1;
        PyObject *pKey = PyList_GetItem(pKeys, i);
        PyObject *pValue;
        pValue = PyDict_GetItem(value, pKey);
        v = do_strdup_printf("%ld:%f",  MyLong_AsLong(pKey), PyFloat_AsDouble(pValue));
        if ( buf ) {
            v1 = do_strdup_printf("%s,%s", buf, v);
            do_free(buf);do_free(v);
            buf = v1;
        }
        else 
            buf = v;
    }
    if ( buf ) {
       char *v;
       v = do_strdup_printf("%s:[%s]", name, buf);
       do_free(buf);
       buf = v;
    }
    else
       buf=do_strdup_printf("%s:[]",name);
    return buf;
}
char *get_params_desc(PyObject *value) 
{
    char *buf;
    PyObject *pKeys = PyDict_Keys(value);
    int i;
    buf = NULL;
    for ( i = 0; i < PyList_Size(pKeys); ++i ) {
        char *v,*v1;
        PyObject *pKey = PyList_GetItem(pKeys, i);
        PyObject *pValue;
        pValue = PyDict_GetItem(value, pKey);
        char *name;char *val;
        name = MyString_AsString(pKey);
        val = MyString_AsString(pValue);
        v = do_strdup_printf("%s:\"%s\"", name, val);
        if ( buf ) {
            v1 = do_strdup_printf("%s,%s", buf, v);
            do_free(buf);do_free(v);
            buf = v1;
        }
        else 
            buf = v;
        do_free(name);do_free(val);
    }
    if ( buf ) {
       char *v;
       v = do_strdup_printf("Params:[%s]", buf);
       do_free(buf);
       buf = v;
    }
    else
       buf=do_strdup("Params:[]");
    return buf;
}
char *MyString_AsString(PyObject *value)
{
#if PY_MAJOR_VERSION >= 3
    if ( PyUnicode_Check(value) ) {
        PyObject *tmp;
        char *retval;
        tmp = PyUnicode_AsUTF8String(value);
        retval = strdup(PyBytes_AsString(tmp));
        Py_DECREF(tmp);
        return retval;
    }
    return strdup("");
#else 
    if ( PyString_Check(value) ) 
        return strdup(PyString_AsString(value));
    
    if ( PyUnicode_Check(value) ) {
        PyObject *tmp;
        char *retval;
        tmp = PyUnicode_AsUTF8String(value);
        retval = strdup(PyString_AsString(tmp));
        Py_DECREF(tmp);
        return retval;
    }
    return strdup("");
#endif    
}
PyObject *MyString_FromString(const char *value)
{
#if PY_MAJOR_VERSION >= 3
    return PyUnicode_DecodeUTF8(value,strlen(value),NULL);
#else
    return PyString_FromString(value);
#endif
}
/*
#define MAX_LOG_LINE 1025
PyObject *do_log(LOG_ERR, const char *msgfmt, ...)
{
    va_list ap;
    va_start(ap, msgfmt);
    char message[MAX_LOG_LINE+1];
    int len = 0;
    len = vsnprintf(message, MAX_LOG_LINE, msgfmt, ap);
    if (len > MAX_LOG_LINE)
        message[MAX_LOG_LINE] = '\0';
    else
        message[len] = '\0';
    va_end(ap);
//    fprintf(stderr, "%s\n", message);
    PyErr_SetString(Error, message);
    return NULL;
}
*/
