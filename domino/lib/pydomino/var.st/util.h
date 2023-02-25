
#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#ifdef _DEBUG
#define LOG(...) fprintf (stderr, __VA_ARGS__)
#else
#define LOG(...)
#endif // _DEBUG

#if PY_MAJOR_VERSION >= 3
#define MyString_Check(x) PyUnicode_Check(x)
#define MyLong_Check(x) PyLong_Check(x)
#define MyLong_AsLong(x) PyLong_AsLong(x)
#define MyLong_FromLong(x) PyLong_FromLong(x)
#else
#define MyString_Check(x) (PyUnicode_Check(x) || PyString_Check(x))
#define MyLong_Check(x) (PyLong_Check(x) || PyInt_Check(x))
#define MyLong_AsLong(x) (PyLong_Check(x) ? PyLong_AsLong(x) : PyInt_AsLong(x))
#define MyLong_FromLong(x) PyLong_FromLong(x)
#endif


#ifdef __cplusplus
extern "C"
{
#endif

PyObject* do_getparams(const char* src);
char *get_desc(const char *name, PyObject *value);
char *get_prices_desc(PyObject *value);
char *get_quants_desc(PyObject *value, const char* name);
char *get_params_desc(PyObject *value);
char *MyString_AsString(PyObject *value);
PyObject *MyString_FromString(const char *value);
//PyObject *do_log(LOG_ERR, const char *msgfmt, ...);
PyObject *getDominoErrorObject();
#ifdef USE_LOUDMOTH 
PyObject *send_xmpp_message(PyObject *args, PyObject *kwds);
#endif

#ifdef __cplusplus
}
#endif

#endif // UTIL_H_INCLUDED

