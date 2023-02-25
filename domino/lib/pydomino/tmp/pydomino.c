
#include <Python.h>
#include <domino.h>
#include <locale.h>
#include <datetime.h>
#include "util.h"
#include "db.h"
#include "../src/head.h"
#include "../version.h"


static PyObject *Get_local_store(Saldo* self, void *unused)
{
    PyObject *result;

    result = MyString_FromString(domino_unit());
    return result;
}
static PyObject *Get_domino_date(PyObject *obj_, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"value", NULL};
    PyObject *value=0;
    PyDateTime_IMPORT;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O", kwlist, &value) ) {
        do_log(LOG_ERR, "no param value");
        return NULL;
    }
    PyObject *result;
    struct tm tm;
    BTI_LONG date;
    if ( PyDate_Check(value) || PyDateTime_Check(value) ) {
        do_date_set_ymd(&tm, PyDateTime_GET_YEAR(value), PyDateTime_GET_MONTH(value), PyDateTime_GET_DAY(value));
        do_date_set(&date, tm);
        result = MyLong_FromLong(date);
    }
    else {
        if ( MyLong_Check(value) ) {
            struct tm tm;
            date = MyLong_AsLong(value);
            do_date(date, &tm);
            result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);
        }
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type.");
            return NULL;
        }
    }
    return result;
}
static PyObject *Get_domino_time(PyObject *obj_, PyObject *args, PyObject *kwds)
{

    static char *kwlist[] = {"value", NULL};
    PyObject *value=0;
    PyDateTime_IMPORT;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O", kwlist, &value) ) {
        do_log(LOG_ERR, "no param value");
        return NULL;
    }
    PyObject *result;
    struct tm tm;
    BTI_LONG date;
    if ( PyTime_Check(value) || PyDateTime_Check(value) ) {
        if ( PyTime_Check(value) ) {
             tm.tm_hour = PyDateTime_TIME_GET_HOUR(value);tm.tm_min=PyDateTime_TIME_GET_MINUTE(value);tm.tm_sec=PyDateTime_TIME_GET_SECOND(value);
        }
        else  {
             tm.tm_hour = PyDateTime_DATE_GET_HOUR(value);tm.tm_min=PyDateTime_DATE_GET_MINUTE(value);tm.tm_sec=PyDateTime_DATE_GET_SECOND(value);
        }
        do_time_set(&date, tm);
        result = MyLong_FromLong(date);
    }
    else {
        if ( MyLong_Check(value) ) {
            struct tm tm;
            date = MyLong_AsLong(value);
            do_time(date, &tm);
            result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);
        }
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type.");
            return NULL;
        }
    }
    return result;
}

static PyMethodDef module_methods[] = {
	{"local_store", (PyCFunction)Get_local_store, METH_NOARGS, "local_store"},
	{"domino_date", (PyCFunction)Get_domino_date, METH_VARARGS|METH_KEYWORDS, "domino_date"},
	{"domino_time", (PyCFunction)Get_domino_time, METH_VARARGS|METH_KEYWORDS, "domino_time"},
    {NULL}  /* Sentinel */
};

//static PyObject *DominoError;

/*static PyObject *do_log(LOG_ERR, const char *mgs, int level, void *user_data)
{
    fprintf(stderr, "%s\n", mgs);
    if ( level == LOG_ERR || level == LOG_CRIT ) {
        PyErr_SetString(DominoError, mgs);
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}*/
static int log_func(const char *mgs, int level, void *user_data)
{
    if ( level == LOG_ERR || level == LOG_CRIT ) {
        PyErr_SetString(getDominoErrorObject(), mgs);
        return FALSE;
    }
    else {
        fprintf(stderr, "%s\n", mgs);
    }
    return TRUE;
}

#if PY_MAJOR_VERSION >= 3
static PyModuleDef PyDomino_module = {
    PyModuleDef_HEAD_INIT,      /* m_base */
    "pydomino",                 /* m_name */
    NULL,                       /* m_doc */
    -1,                         /* m_size */
    module_methods,             /* m_methods */
};
#endif
PyMODINIT_FUNC
#if PY_MAJOR_VERSION >= 3
PyInit_pydomino(void)
#else
initpydomino(void)
#endif
{
    PyObject* m;

    if ( PyType_Ready(getAliasType()) < 0 )
#if PY_MAJOR_VERSION >= 3
        return NULL;
#else
        return;
#endif         
    setlocale(LC_ALL, "");
    
#if PY_MAJOR_VERSION >= 3
    m = PyModule_Create(&PyDomino_module);
#else    
    m = Py_InitModule3("pydomino", module_methods,
                       "Example module that creates an extension type.");
#endif                       

    if (m == NULL)
#if PY_MAJOR_VERSION >= 3
        return NULL;
#else
        return;
#endif         

    //PyObject *DominoError = getDominoErrorObject();
    //PyErr_NewException("pydomino.error", NULL, NULL);
    //Py_INCREF(DominoError);
    PyModule_AddObject(m, "error", getDominoErrorObject());

    do_log_set_log_func(log_func, NULL);
    //do_log_set_stdout();
    //LOG("domino init\n");

    if ( !domino_init(NULL, TRUE, DO_PARAM_END) )
         goto init_error;
    Py_INCREF(getAliasType());
    PyModule_AddObject(m, "Alias", (PyObject *)getAliasType());
    PyModule_AddStringConstant(m, "__version__", VERSION " " ADATE);
    


    if ( PyType_Ready(getDocumentType()) < 0 )
         goto init_error;

    Py_INCREF(getDocumentType());

    PyModule_AddObject(m, "Document", (PyObject *)getDocumentType());

    if ( PyType_Ready(getDocumentKey0Type()) < 0 )
         goto init_error;

    Py_INCREF(getDocumentKey0Type());

    PyModule_AddObject(m, "DocumentKey0", (PyObject *)getDocumentKey0Type());

    if ( PyType_Ready(getDocumentKey1Type()) < 0 )
         goto init_error;

    Py_INCREF(getDocumentKey1Type());

    PyModule_AddObject(m, "DocumentKey1", (PyObject *)getDocumentKey1Type());

    if ( PyType_Ready(getDocumentKey2Type()) < 0 )
         goto init_error;

    Py_INCREF(getDocumentKey2Type());

    PyModule_AddObject(m, "DocumentKey2", (PyObject *)getDocumentKey2Type());

    if ( PyType_Ready(getDocumentKey3Type()) < 0 )
         goto init_error;

    Py_INCREF(getDocumentKey3Type());

    PyModule_AddObject(m, "DocumentKey3", (PyObject *)getDocumentKey3Type());

    if ( PyType_Ready(getDocumentKey4Type()) < 0 )
         goto init_error;

    Py_INCREF(getDocumentKey4Type());

    PyModule_AddObject(m, "DocumentKey4", (PyObject *)getDocumentKey4Type());

    if ( PyType_Ready(getDocumentOrderType()) < 0 )
         goto init_error;

    Py_INCREF(getDocumentOrderType());

    PyModule_AddObject(m, "DocumentOrder", (PyObject *)getDocumentOrderType());

    if ( PyType_Ready(getDocumentOrderKey0Type()) < 0 )
         goto init_error;

    Py_INCREF(getDocumentOrderKey0Type());

    PyModule_AddObject(m, "DocumentOrderKey0", (PyObject *)getDocumentOrderKey0Type());

    if ( PyType_Ready(getDocumentOrderKey1Type()) < 0 )
         goto init_error;

    Py_INCREF(getDocumentOrderKey1Type());

    PyModule_AddObject(m, "DocumentOrderKey1", (PyObject *)getDocumentOrderKey1Type());

    if ( PyType_Ready(getDocumentOrderKey2Type()) < 0 )
         goto init_error;

    Py_INCREF(getDocumentOrderKey2Type());

    PyModule_AddObject(m, "DocumentOrderKey2", (PyObject *)getDocumentOrderKey2Type());

    if ( PyType_Ready(getDocumentOrderKey3Type()) < 0 )
         goto init_error;

    Py_INCREF(getDocumentOrderKey3Type());

    PyModule_AddObject(m, "DocumentOrderKey3", (PyObject *)getDocumentOrderKey3Type());

    if ( PyType_Ready(getDocumentOrderKey4Type()) < 0 )
         goto init_error;

    Py_INCREF(getDocumentOrderKey4Type());

    PyModule_AddObject(m, "DocumentOrderKey4", (PyObject *)getDocumentOrderKey4Type());

    if ( PyType_Ready(getDocumentLinkType()) < 0 )
         goto init_error;

    Py_INCREF(getDocumentLinkType());

    PyModule_AddObject(m, "DocumentLink", (PyObject *)getDocumentLinkType());

    if ( PyType_Ready(getDocumentLinkKey0Type()) < 0 )
         goto init_error;

    Py_INCREF(getDocumentLinkKey0Type());

    PyModule_AddObject(m, "DocumentLinkKey0", (PyObject *)getDocumentLinkKey0Type());

    if ( PyType_Ready(getDocumentLinkKey1Type()) < 0 )
         goto init_error;

    Py_INCREF(getDocumentLinkKey1Type());

    PyModule_AddObject(m, "DocumentLinkKey1", (PyObject *)getDocumentLinkKey1Type());

    if ( PyType_Ready(getEnumType()) < 0 )
         goto init_error;

    Py_INCREF(getEnumType());

    PyModule_AddObject(m, "Enum", (PyObject *)getEnumType());

    if ( PyType_Ready(getEnumKey0Type()) < 0 )
         goto init_error;

    Py_INCREF(getEnumKey0Type());

    PyModule_AddObject(m, "EnumKey0", (PyObject *)getEnumKey0Type());

    if ( PyType_Ready(getDocumentViewType()) < 0 )
         goto init_error;

    Py_INCREF(getDocumentViewType());

    PyModule_AddObject(m, "DocumentView", (PyObject *)getDocumentViewType());

    if ( PyType_Ready(getDocumentViewKey0Type()) < 0 )
         goto init_error;

    Py_INCREF(getDocumentViewKey0Type());

    PyModule_AddObject(m, "DocumentViewKey0", (PyObject *)getDocumentViewKey0Type());

    if ( PyType_Ready(getDocumentViewKey1Type()) < 0 )
         goto init_error;

    Py_INCREF(getDocumentViewKey1Type());

    PyModule_AddObject(m, "DocumentViewKey1", (PyObject *)getDocumentViewKey1Type());

    if ( PyType_Ready(getDocumentDataType()) < 0 )
         goto init_error;

    Py_INCREF(getDocumentDataType());

    PyModule_AddObject(m, "DocumentData", (PyObject *)getDocumentDataType());

    if ( PyType_Ready(getDocumentDataKey0Type()) < 0 )
         goto init_error;

    Py_INCREF(getDocumentDataKey0Type());

    PyModule_AddObject(m, "DocumentDataKey0", (PyObject *)getDocumentDataKey0Type());

    if ( PyType_Ready(getDocumentDataKey1Type()) < 0 )
         goto init_error;

    Py_INCREF(getDocumentDataKey1Type());

    PyModule_AddObject(m, "DocumentDataKey1", (PyObject *)getDocumentDataKey1Type());

    if ( PyType_Ready(getDocumentDataKey2Type()) < 0 )
         goto init_error;

    Py_INCREF(getDocumentDataKey2Type());

    PyModule_AddObject(m, "DocumentDataKey2", (PyObject *)getDocumentDataKey2Type());

    if ( PyType_Ready(getProductType()) < 0 )
         goto init_error;

    Py_INCREF(getProductType());

    PyModule_AddObject(m, "Product", (PyObject *)getProductType());

    if ( PyType_Ready(getProductKey0Type()) < 0 )
         goto init_error;

    Py_INCREF(getProductKey0Type());

    PyModule_AddObject(m, "ProductKey0", (PyObject *)getProductKey0Type());

    if ( PyType_Ready(getProductKey1Type()) < 0 )
         goto init_error;

    Py_INCREF(getProductKey1Type());

    PyModule_AddObject(m, "ProductKey1", (PyObject *)getProductKey1Type());

    if ( PyType_Ready(getProductKey2Type()) < 0 )
         goto init_error;

    Py_INCREF(getProductKey2Type());

    PyModule_AddObject(m, "ProductKey2", (PyObject *)getProductKey2Type());

    if ( PyType_Ready(getProductKey3Type()) < 0 )
         goto init_error;

    Py_INCREF(getProductKey3Type());

    PyModule_AddObject(m, "ProductKey3", (PyObject *)getProductKey3Type());

    if ( PyType_Ready(getProductKey4Type()) < 0 )
         goto init_error;

    Py_INCREF(getProductKey4Type());

    PyModule_AddObject(m, "ProductKey4", (PyObject *)getProductKey4Type());

    if ( PyType_Ready(getProductLinkType()) < 0 )
         goto init_error;

    Py_INCREF(getProductLinkType());

    PyModule_AddObject(m, "ProductLink", (PyObject *)getProductLinkType());

    if ( PyType_Ready(getProductLinkKey0Type()) < 0 )
         goto init_error;

    Py_INCREF(getProductLinkKey0Type());

    PyModule_AddObject(m, "ProductLinkKey0", (PyObject *)getProductLinkKey0Type());

    if ( PyType_Ready(getProductLinkKey1Type()) < 0 )
         goto init_error;

    Py_INCREF(getProductLinkKey1Type());

    PyModule_AddObject(m, "ProductLinkKey1", (PyObject *)getProductLinkKey1Type());

    if ( PyType_Ready(getProductViewType()) < 0 )
         goto init_error;

    Py_INCREF(getProductViewType());

    PyModule_AddObject(m, "ProductView", (PyObject *)getProductViewType());

    if ( PyType_Ready(getProductViewKey0Type()) < 0 )
         goto init_error;

    Py_INCREF(getProductViewKey0Type());

    PyModule_AddObject(m, "ProductViewKey0", (PyObject *)getProductViewKey0Type());

    if ( PyType_Ready(getProductViewKey1Type()) < 0 )
         goto init_error;

    Py_INCREF(getProductViewKey1Type());

    PyModule_AddObject(m, "ProductViewKey1", (PyObject *)getProductViewKey1Type());

    if ( PyType_Ready(getProductDataType()) < 0 )
         goto init_error;

    Py_INCREF(getProductDataType());

    PyModule_AddObject(m, "ProductData", (PyObject *)getProductDataType());

    if ( PyType_Ready(getProductDataKey0Type()) < 0 )
         goto init_error;

    Py_INCREF(getProductDataKey0Type());

    PyModule_AddObject(m, "ProductDataKey0", (PyObject *)getProductDataKey0Type());

    if ( PyType_Ready(getProductDataKey1Type()) < 0 )
         goto init_error;

    Py_INCREF(getProductDataKey1Type());

    PyModule_AddObject(m, "ProductDataKey1", (PyObject *)getProductDataKey1Type());

    if ( PyType_Ready(getProductDataKey2Type()) < 0 )
         goto init_error;

    Py_INCREF(getProductDataKey2Type());

    PyModule_AddObject(m, "ProductDataKey2", (PyObject *)getProductDataKey2Type());

    if ( PyType_Ready(getBarcodeType()) < 0 )
         goto init_error;

    Py_INCREF(getBarcodeType());

    PyModule_AddObject(m, "Barcode", (PyObject *)getBarcodeType());

    if ( PyType_Ready(getBarcodeKey0Type()) < 0 )
         goto init_error;

    Py_INCREF(getBarcodeKey0Type());

    PyModule_AddObject(m, "BarcodeKey0", (PyObject *)getBarcodeKey0Type());

    if ( PyType_Ready(getBarcodeKey1Type()) < 0 )
         goto init_error;

    Py_INCREF(getBarcodeKey1Type());

    PyModule_AddObject(m, "BarcodeKey1", (PyObject *)getBarcodeKey1Type());

    if ( PyType_Ready(getAccountType()) < 0 )
         goto init_error;

    Py_INCREF(getAccountType());

    PyModule_AddObject(m, "Account", (PyObject *)getAccountType());

    if ( PyType_Ready(getAccountKey0Type()) < 0 )
         goto init_error;

    Py_INCREF(getAccountKey0Type());

    PyModule_AddObject(m, "AccountKey0", (PyObject *)getAccountKey0Type());

    if ( PyType_Ready(getSubaccountType()) < 0 )
         goto init_error;

    Py_INCREF(getSubaccountType());

    PyModule_AddObject(m, "Subaccount", (PyObject *)getSubaccountType());

    if ( PyType_Ready(getSubaccountKey0Type()) < 0 )
         goto init_error;

    Py_INCREF(getSubaccountKey0Type());

    PyModule_AddObject(m, "SubaccountKey0", (PyObject *)getSubaccountKey0Type());

    if ( PyType_Ready(getClassType()) < 0 )
         goto init_error;

    Py_INCREF(getClassType());

    PyModule_AddObject(m, "Class", (PyObject *)getClassType());

    if ( PyType_Ready(getClassKey0Type()) < 0 )
         goto init_error;

    Py_INCREF(getClassKey0Type());

    PyModule_AddObject(m, "ClassKey0", (PyObject *)getClassKey0Type());

    if ( PyType_Ready(getGroupType()) < 0 )
         goto init_error;

    Py_INCREF(getGroupType());

    PyModule_AddObject(m, "Group", (PyObject *)getGroupType());

    if ( PyType_Ready(getGroupKey0Type()) < 0 )
         goto init_error;

    Py_INCREF(getGroupKey0Type());

    PyModule_AddObject(m, "GroupKey0", (PyObject *)getGroupKey0Type());

    if ( PyType_Ready(getGroupKey1Type()) < 0 )
         goto init_error;

    Py_INCREF(getGroupKey1Type());

    PyModule_AddObject(m, "GroupKey1", (PyObject *)getGroupKey1Type());

    if ( PyType_Ready(getSubgroupType()) < 0 )
         goto init_error;

    Py_INCREF(getSubgroupType());

    PyModule_AddObject(m, "Subgroup", (PyObject *)getSubgroupType());

    if ( PyType_Ready(getSubgroupKey0Type()) < 0 )
         goto init_error;

    Py_INCREF(getSubgroupKey0Type());

    PyModule_AddObject(m, "SubgroupKey0", (PyObject *)getSubgroupKey0Type());

    if ( PyType_Ready(getSubgroupKey1Type()) < 0 )
         goto init_error;

    Py_INCREF(getSubgroupKey1Type());

    PyModule_AddObject(m, "SubgroupKey1", (PyObject *)getSubgroupKey1Type());

    if ( PyType_Ready(getRegionType()) < 0 )
         goto init_error;

    Py_INCREF(getRegionType());

    PyModule_AddObject(m, "Region", (PyObject *)getRegionType());

    if ( PyType_Ready(getRegionKey0Type()) < 0 )
         goto init_error;

    Py_INCREF(getRegionKey0Type());

    PyModule_AddObject(m, "RegionKey0", (PyObject *)getRegionKey0Type());

    if ( PyType_Ready(getRegionKey1Type()) < 0 )
         goto init_error;

    Py_INCREF(getRegionKey1Type());

    PyModule_AddObject(m, "RegionKey1", (PyObject *)getRegionKey1Type());

    if ( PyType_Ready(getDivisionType()) < 0 )
         goto init_error;

    Py_INCREF(getDivisionType());

    PyModule_AddObject(m, "Division", (PyObject *)getDivisionType());

    if ( PyType_Ready(getDivisionKey0Type()) < 0 )
         goto init_error;

    Py_INCREF(getDivisionKey0Type());

    PyModule_AddObject(m, "DivisionKey0", (PyObject *)getDivisionKey0Type());

    if ( PyType_Ready(getPartnerType()) < 0 )
         goto init_error;

    Py_INCREF(getPartnerType());

    PyModule_AddObject(m, "Partner", (PyObject *)getPartnerType());

    if ( PyType_Ready(getPartnerKey0Type()) < 0 )
         goto init_error;

    Py_INCREF(getPartnerKey0Type());

    PyModule_AddObject(m, "PartnerKey0", (PyObject *)getPartnerKey0Type());

    if ( PyType_Ready(getPartnerKey1Type()) < 0 )
         goto init_error;

    Py_INCREF(getPartnerKey1Type());

    PyModule_AddObject(m, "PartnerKey1", (PyObject *)getPartnerKey1Type());

    if ( PyType_Ready(getPartnerKey2Type()) < 0 )
         goto init_error;

    Py_INCREF(getPartnerKey2Type());

    PyModule_AddObject(m, "PartnerKey2", (PyObject *)getPartnerKey2Type());

    if ( PyType_Ready(getPartnerKey3Type()) < 0 )
         goto init_error;

    Py_INCREF(getPartnerKey3Type());

    PyModule_AddObject(m, "PartnerKey3", (PyObject *)getPartnerKey3Type());

    if ( PyType_Ready(getPartnerDataType()) < 0 )
         goto init_error;

    Py_INCREF(getPartnerDataType());

    PyModule_AddObject(m, "PartnerData", (PyObject *)getPartnerDataType());

    if ( PyType_Ready(getPartnerDataKey0Type()) < 0 )
         goto init_error;

    Py_INCREF(getPartnerDataKey0Type());

    PyModule_AddObject(m, "PartnerDataKey0", (PyObject *)getPartnerDataKey0Type());

    if ( PyType_Ready(getPartnerDataKey1Type()) < 0 )
         goto init_error;

    Py_INCREF(getPartnerDataKey1Type());

    PyModule_AddObject(m, "PartnerDataKey1", (PyObject *)getPartnerDataKey1Type());

    if ( PyType_Ready(getPartnerDataKey2Type()) < 0 )
         goto init_error;

    Py_INCREF(getPartnerDataKey2Type());

    PyModule_AddObject(m, "PartnerDataKey2", (PyObject *)getPartnerDataKey2Type());

    if ( PyType_Ready(getStoreType()) < 0 )
         goto init_error;

    Py_INCREF(getStoreType());

    PyModule_AddObject(m, "Store", (PyObject *)getStoreType());

    if ( PyType_Ready(getStoreKey0Type()) < 0 )
         goto init_error;

    Py_INCREF(getStoreKey0Type());

    PyModule_AddObject(m, "StoreKey0", (PyObject *)getStoreKey0Type());

    if ( PyType_Ready(getDocumentProwType()) < 0 )
         goto init_error;

    Py_INCREF(getDocumentProwType());

    PyModule_AddObject(m, "DocumentProw", (PyObject *)getDocumentProwType());

    if ( PyType_Ready(getDocumentProwKey0Type()) < 0 )
         goto init_error;

    Py_INCREF(getDocumentProwKey0Type());

    PyModule_AddObject(m, "DocumentProwKey0", (PyObject *)getDocumentProwKey0Type());

    if ( PyType_Ready(getDocumentTypeType()) < 0 )
         goto init_error;

    Py_INCREF(getDocumentTypeType());

    PyModule_AddObject(m, "DocumentType", (PyObject *)getDocumentTypeType());

    if ( PyType_Ready(getDocumentTypeKey0Type()) < 0 )
         goto init_error;

    Py_INCREF(getDocumentTypeKey0Type());

    PyModule_AddObject(m, "DocumentTypeKey0", (PyObject *)getDocumentTypeKey0Type());

    if ( PyType_Ready(getStockType()) < 0 )
         goto init_error;

    Py_INCREF(getStockType());

    PyModule_AddObject(m, "Stock", (PyObject *)getStockType());

    if ( PyType_Ready(getStockKey0Type()) < 0 )
         goto init_error;

    Py_INCREF(getStockKey0Type());

    PyModule_AddObject(m, "StockKey0", (PyObject *)getStockKey0Type());

    if ( PyType_Ready(getStockKey1Type()) < 0 )
         goto init_error;

    Py_INCREF(getStockKey1Type());

    PyModule_AddObject(m, "StockKey1", (PyObject *)getStockKey1Type());

    if ( PyType_Ready(getStockKey2Type()) < 0 )
         goto init_error;

    Py_INCREF(getStockKey2Type());

    PyModule_AddObject(m, "StockKey2", (PyObject *)getStockKey2Type());

    if ( PyType_Ready(getStockKey3Type()) < 0 )
         goto init_error;

    Py_INCREF(getStockKey3Type());

    PyModule_AddObject(m, "StockKey3", (PyObject *)getStockKey3Type());

    if ( PyType_Ready(getProwodType()) < 0 )
         goto init_error;

    Py_INCREF(getProwodType());

    PyModule_AddObject(m, "Prowod", (PyObject *)getProwodType());

    if ( PyType_Ready(getProwodKey0Type()) < 0 )
         goto init_error;

    Py_INCREF(getProwodKey0Type());

    PyModule_AddObject(m, "ProwodKey0", (PyObject *)getProwodKey0Type());

    if ( PyType_Ready(getProwodKey1Type()) < 0 )
         goto init_error;

    Py_INCREF(getProwodKey1Type());

    PyModule_AddObject(m, "ProwodKey1", (PyObject *)getProwodKey1Type());

    if ( PyType_Ready(getProwodKey2Type()) < 0 )
         goto init_error;

    Py_INCREF(getProwodKey2Type());

    PyModule_AddObject(m, "ProwodKey2", (PyObject *)getProwodKey2Type());

    if ( PyType_Ready(getProwodKey3Type()) < 0 )
         goto init_error;

    Py_INCREF(getProwodKey3Type());

    PyModule_AddObject(m, "ProwodKey3", (PyObject *)getProwodKey3Type());

    if ( PyType_Ready(getProwodKey4Type()) < 0 )
         goto init_error;

    Py_INCREF(getProwodKey4Type());

    PyModule_AddObject(m, "ProwodKey4", (PyObject *)getProwodKey4Type());

    if ( PyType_Ready(getProwod2Type()) < 0 )
         goto init_error;

    Py_INCREF(getProwod2Type());

    PyModule_AddObject(m, "Prowod2", (PyObject *)getProwod2Type());

    if ( PyType_Ready(getProwod2Key0Type()) < 0 )
         goto init_error;

    Py_INCREF(getProwod2Key0Type());

    PyModule_AddObject(m, "Prowod2Key0", (PyObject *)getProwod2Key0Type());

    if ( PyType_Ready(getProwod2Key1Type()) < 0 )
         goto init_error;

    Py_INCREF(getProwod2Key1Type());

    PyModule_AddObject(m, "Prowod2Key1", (PyObject *)getProwod2Key1Type());

    if ( PyType_Ready(getProwod2Key2Type()) < 0 )
         goto init_error;

    Py_INCREF(getProwod2Key2Type());

    PyModule_AddObject(m, "Prowod2Key2", (PyObject *)getProwod2Key2Type());

    if ( PyType_Ready(getProwod2Key3Type()) < 0 )
         goto init_error;

    Py_INCREF(getProwod2Key3Type());

    PyModule_AddObject(m, "Prowod2Key3", (PyObject *)getProwod2Key3Type());

    if ( PyType_Ready(getSaldoType()) < 0 )
         goto init_error;

    Py_INCREF(getSaldoType());

    PyModule_AddObject(m, "Saldo", (PyObject *)getSaldoType());

    if ( PyType_Ready(getSaldoKey0Type()) < 0 )
         goto init_error;

    Py_INCREF(getSaldoKey0Type());

    PyModule_AddObject(m, "SaldoKey0", (PyObject *)getSaldoKey0Type());

    if ( PyType_Ready(getProtocolType()) < 0 )
         goto init_error;

    Py_INCREF(getProtocolType());

    PyModule_AddObject(m, "Protocol", (PyObject *)getProtocolType());

    if ( PyType_Ready(getProtocolKey0Type()) < 0 )
         goto init_error;

    Py_INCREF(getProtocolKey0Type());

    PyModule_AddObject(m, "ProtocolKey0", (PyObject *)getProtocolKey0Type());

    if ( PyType_Ready(getProtocolKey1Type()) < 0 )
         goto init_error;

    Py_INCREF(getProtocolKey1Type());

    PyModule_AddObject(m, "ProtocolKey1", (PyObject *)getProtocolKey1Type());

    if ( PyType_Ready(getProtocolKey2Type()) < 0 )
         goto init_error;

    Py_INCREF(getProtocolKey2Type());

    PyModule_AddObject(m, "ProtocolKey2", (PyObject *)getProtocolKey2Type());

    if ( PyType_Ready(getUserType()) < 0 )
         goto init_error;

    Py_INCREF(getUserType());

    PyModule_AddObject(m, "User", (PyObject *)getUserType());

    if ( PyType_Ready(getUserKey0Type()) < 0 )
         goto init_error;

    Py_INCREF(getUserKey0Type());

    PyModule_AddObject(m, "UserKey0", (PyObject *)getUserKey0Type());

#ifndef DOMINO78
    if ( PyType_Ready(getGrlsType()) < 0 )
         goto init_error;
    Py_INCREF(getGrlsType());

    PyModule_AddObject(m, "Grls", (PyObject *)getGrlsType());

    if ( PyType_Ready(getMarkupType()) < 0 )
         goto init_error;

    Py_INCREF(getMarkupType());

    PyModule_AddObject(m, "Markup", (PyObject *)getMarkupType());

    if ( PyType_Ready(getMarkupInfoType()) < 0 )
         goto init_error;

    Py_INCREF(getMarkupInfoType());

    PyModule_AddObject(m, "MarkupInfo", (PyObject *)getMarkupInfoType());

    if ( PyType_Ready(getPriceLimitType()) < 0 )
         goto init_error;

    Py_INCREF(getPriceLimitType());

    PyModule_AddObject(m, "PriceLimit", (PyObject *)getPriceLimitType());

    if ( PyType_Ready(getTrafficType()) < 0 )
         goto init_error;

    Py_INCREF(getTrafficType());

    PyModule_AddObject(m, "Traffic", (PyObject *)getTrafficType());
#endif
    if ( PyType_Ready(getViewType()) < 0 )
         goto init_error;

    Py_INCREF(getViewType());

    PyModule_AddObject(m, "View", (PyObject *)getViewType());

#if PY_MAJOR_VERSION >= 3
    return m;
  init_error:
    Py_CLEAR(m);
    return NULL;
#else
  init_error:
    return;
#endif

}
