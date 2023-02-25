
#include <Python.h>
#include "alias.h"
#include "util.h"
#include "db.h"

static void Alias_dealloc(PyObject* self)
{
    LOG("Alias free %d\n",self);
    if ( ((Alias*)self)->alias )
        do_alias_free(((Alias*)self)->alias);
    ((Alias*)self)->alias = NULL;
    self->ob_type->tp_free((PyObject*)self);
}
static PyObject *Alias_clear(PyObject* self)
{
    LOG("Alias clear\n");
    if ( ((Alias*)self)->alias )
        do_alias_free(((Alias*)self)->alias);
    ((Alias*)self)->alias = NULL;
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *Alias_start_tran(PyObject* self)
{
    LOG("Alias clear\n");
    int res = do_alias_tran_begin( ((Alias*)self)->alias);
    return PyBool_FromLong(res);
}
static PyObject *Alias_end_tran(PyObject* self)
{
    LOG("Alias clear\n");
    int res = do_alias_tran_end( ((Alias*)self)->alias);
    return PyBool_FromLong(res);
}
static PyObject *Alias_abort_tran(PyObject* self)
{
    LOG("Alias clear\n");
    int res = do_alias_tran_abort( ((Alias*)self)->alias);
    return PyBool_FromLong(res);
}
static PyObject *Alias_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Alias *self;

    self = PyObject_NEW(Alias, type);
    LOG("Alias new %d\n",self);
    if ( self == NULL )
        return NULL;
    self->alias = NULL;
    return (PyObject *)self;
}

static int Alias_init(Alias *self, PyObject *args, PyObject *kwds)
{
    //do_log(LOG_ERR,"Alias init\n");

    static char *kwlist[] = {"name", "store", "hostname", "dbname", "alias", NULL};
    char *name = NULL;
    int store = 0; 
    char *hostname = NULL;
    char *dbname = NULL;
    PyObject *other = NULL;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|sissO", kwlist,
                                      &name, &store, &hostname, &dbname, &other) )
        return -1;
    product_struct_t product;
    if ( sizeof(product) != 2470 ) {
	do_log(LOG_ERR, "The library is compiled without __attribute__((packed)) sizeof product struct %d", sizeof(product));
	return -1;
    }
    if ( other ) {
       self->alias = do_alias_clone(((Alias*)other)->alias);
    } 
    else {
        self->alias = domino_alias_new(name);
        if ( !self->alias )
           return -1;
        if ( store > 0 || hostname || dbname )
   	   domino_alias_set_store_number(self->alias, store, hostname, dbname);
    }	
    if ( !do_alias_open(self->alias, TRUE) ) 
        return -1;
    return 0;
}

static PyObject *Alias_connect(Alias *self)
{
    LOG("Alias connect\n");

    if ( !do_alias_open(self->alias, TRUE) )
        return NULL;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Alias_get_name(Alias* self)
{
    PyObject *result;

    result = MyString_FromString(do_alias_get_name(self->alias));

    return result;
}
static PyObject *Alias_get_host(Alias* self)
{
    PyObject *result;

    result = MyString_FromString(do_alias_get_host(self->alias));

    return result;
}
static PyObject *Alias_utf8(Alias* self)
{
    PyObject  *result;

    result = Py_BuildValue("i", do_alias_utf8(self->alias));

    return result;
}
static PyObject *Alias_connected(Alias* self)
{
    PyObject  *result;

    result = Py_BuildValue("i", do_alias_conected(self->alias));

    return result;
}

static PyObject *Alias_Partner(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getPartnerType(),
                                    "O", self);
}   

static PyObject *Alias_PartnerKey0(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getPartnerKey0Type(),
                                    "O", self);
}   

static PyObject *Alias_PartnerKey1(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getPartnerKey1Type(),
                                    "O", self);
}   

static PyObject *Alias_PartnerKey2(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getPartnerKey2Type(),
                                    "O", self);
}   

static PyObject *Alias_PartnerKey3(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getPartnerKey3Type(),
                                    "O", self);
}   

static PyObject *Alias_DocumentType(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getDocumentTypeType(),
                                    "O", self);
}   

static PyObject *Alias_DocumentTypeKey0(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getDocumentTypeKey0Type(),
                                    "O", self);
}   

static PyObject *Alias_Stock(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getStockType(),
                                    "O", self);
}   

static PyObject *Alias_StockKey0(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getStockKey0Type(),
                                    "O", self);
}   

static PyObject *Alias_StockKey1(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getStockKey1Type(),
                                    "O", self);
}   

static PyObject *Alias_StockKey2(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getStockKey2Type(),
                                    "O", self);
}   

static PyObject *Alias_StockKey3(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getStockKey3Type(),
                                    "O", self);
}   

static PyObject *Alias_Subaccount(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getSubaccountType(),
                                    "O", self);
}   

static PyObject *Alias_SubaccountKey0(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getSubaccountKey0Type(),
                                    "O", self);
}   

static PyObject *Alias_Saldo(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getSaldoType(),
                                    "O", self);
}   

static PyObject *Alias_SaldoKey0(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getSaldoKey0Type(),
                                    "O", self);
}   

static PyObject *Alias_ProductView(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getProductViewType(),
                                    "O", self);
}   

static PyObject *Alias_ProductViewKey0(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getProductViewKey0Type(),
                                    "O", self);
}   

static PyObject *Alias_ProductViewKey1(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getProductViewKey1Type(),
                                    "O", self);
}   

static PyObject *Alias_Group(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getGroupType(),
                                    "O", self);
}   

static PyObject *Alias_GroupKey0(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getGroupKey0Type(),
                                    "O", self);
}   

static PyObject *Alias_GroupKey1(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getGroupKey1Type(),
                                    "O", self);
}   

static PyObject *Alias_User(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getUserType(),
                                    "O", self);
}   

static PyObject *Alias_UserKey0(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getUserKey0Type(),
                                    "O", self);
}   
#ifndef DOMINO78
static PyObject *Alias_Realization(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getRealizationType(),
                                    "O", self);
}   

static PyObject *Alias_RealizationKey0(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getRealizationKey0Type(),
                                    "O", self);
}   

static PyObject *Alias_RealizationKey1(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getRealizationKey1Type(),
                                    "O", self);
}   

static PyObject *Alias_RealizationKey2(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getRealizationKey2Type(),
                                    "O", self);
}   

static PyObject *Alias_RealizationKey3(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getRealizationKey3Type(),
                                    "O", self);
}   

static PyObject *Alias_RealizationKey4(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getRealizationKey4Type(),
                                    "O", self);
}   
#endif
static PyObject *Alias_Store(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getStoreType(),
                                    "O", self);
}   

static PyObject *Alias_StoreKey0(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getStoreKey0Type(),
                                    "O", self);
}   

static PyObject *Alias_Enum(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getEnumType(),
                                    "O", self);
}   

static PyObject *Alias_EnumKey0(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getEnumKey0Type(),
                                    "O", self);
}   

static PyObject *Alias_DocumentProw(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getDocumentProwType(),
                                    "O", self);
}   

static PyObject *Alias_DocumentProwKey0(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getDocumentProwKey0Type(),
                                    "O", self);
}   

static PyObject *Alias_Barcode(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getBarcodeType(),
                                    "O", self);
}   

static PyObject *Alias_BarcodeKey0(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getBarcodeKey0Type(),
                                    "O", self);
}   

static PyObject *Alias_BarcodeKey1(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getBarcodeKey1Type(),
                                    "O", self);
}   

static PyObject *Alias_Prowod2(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getProwod2Type(),
                                    "O", self);
}   

static PyObject *Alias_Prowod2Key0(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getProwod2Key0Type(),
                                    "O", self);
}   

static PyObject *Alias_Prowod2Key1(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getProwod2Key1Type(),
                                    "O", self);
}   

static PyObject *Alias_Prowod2Key2(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getProwod2Key2Type(),
                                    "O", self);
}   

static PyObject *Alias_Prowod2Key3(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getProwod2Key3Type(),
                                    "O", self);
}   

static PyObject *Alias_DocumentData(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getDocumentDataType(),
                                    "O", self);
}   

static PyObject *Alias_DocumentDataKey0(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getDocumentDataKey0Type(),
                                    "O", self);
}   

static PyObject *Alias_DocumentDataKey1(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getDocumentDataKey1Type(),
                                    "O", self);
}   

static PyObject *Alias_DocumentDataKey2(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getDocumentDataKey2Type(),
                                    "O", self);
}   

#ifndef DOMINO78
static PyObject *Alias_Shift(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getShiftType(),
                                    "O", self);
}   

static PyObject *Alias_ShiftKey0(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getShiftKey0Type(),
                                    "O", self);
}   

static PyObject *Alias_ShiftKey1(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getShiftKey1Type(),
                                    "O", self);
}   

static PyObject *Alias_ShiftKey2(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getShiftKey2Type(),
                                    "O", self);
}   

static PyObject *Alias_ShiftKey3(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getShiftKey3Type(),
                                    "O", self);
}   
#endif
static PyObject *Alias_DocumentLink(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getDocumentLinkType(),
                                    "O", self);
}   

static PyObject *Alias_DocumentLinkKey0(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getDocumentLinkKey0Type(),
                                    "O", self);
}   

static PyObject *Alias_DocumentLinkKey1(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getDocumentLinkKey1Type(),
                                    "O", self);
}   

static PyObject *Alias_PartnerData(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getPartnerDataType(),
                                    "O", self);
}   

static PyObject *Alias_PartnerDataKey0(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getPartnerDataKey0Type(),
                                    "O", self);
}   

static PyObject *Alias_PartnerDataKey1(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getPartnerDataKey1Type(),
                                    "O", self);
}   

static PyObject *Alias_PartnerDataKey2(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getPartnerDataKey2Type(),
                                    "O", self);
}   

static PyObject *Alias_Protocol(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getProtocolType(),
                                    "O", self);
}   

static PyObject *Alias_ProtocolKey0(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getProtocolKey0Type(),
                                    "O", self);
}   

static PyObject *Alias_ProtocolKey1(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getProtocolKey1Type(),
                                    "O", self);
}   

static PyObject *Alias_ProtocolKey2(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getProtocolKey2Type(),
                                    "O", self);
}   

static PyObject *Alias_Division(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getDivisionType(),
                                    "O", self);
}   

static PyObject *Alias_DivisionKey0(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getDivisionKey0Type(),
                                    "O", self);
}   

static PyObject *Alias_Document(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getDocumentType(),
                                    "O", self);
}   

static PyObject *Alias_DocumentKey0(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getDocumentKey0Type(),
                                    "O", self);
}   

static PyObject *Alias_DocumentKey1(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getDocumentKey1Type(),
                                    "O", self);
}   

static PyObject *Alias_DocumentKey2(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getDocumentKey2Type(),
                                    "O", self);
}   

static PyObject *Alias_DocumentKey3(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getDocumentKey3Type(),
                                    "O", self);
}   

static PyObject *Alias_DocumentKey4(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getDocumentKey4Type(),
                                    "O", self);
}   

static PyObject *Alias_DocumentOrder(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getDocumentOrderType(),
                                    "O", self);
}   

static PyObject *Alias_DocumentOrderKey0(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getDocumentOrderKey0Type(),
                                    "O", self);
}   

static PyObject *Alias_DocumentOrderKey1(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getDocumentOrderKey1Type(),
                                    "O", self);
}   

static PyObject *Alias_DocumentOrderKey2(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getDocumentOrderKey2Type(),
                                    "O", self);
}   

static PyObject *Alias_DocumentOrderKey3(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getDocumentOrderKey3Type(),
                                    "O", self);
}   

static PyObject *Alias_DocumentOrderKey4(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getDocumentOrderKey4Type(),
                                    "O", self);
}   

static PyObject *Alias_ProductLink(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getProductLinkType(),
                                    "O", self);
}   

static PyObject *Alias_ProductLinkKey0(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getProductLinkKey0Type(),
                                    "O", self);
}   

static PyObject *Alias_ProductLinkKey1(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getProductLinkKey1Type(),
                                    "O", self);
}   

static PyObject *Alias_Class(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getClassType(),
                                    "O", self);
}   

static PyObject *Alias_ClassKey0(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getClassKey0Type(),
                                    "O", self);
}   

static PyObject *Alias_Region(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getRegionType(),
                                    "O", self);
}   

static PyObject *Alias_RegionKey0(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getRegionKey0Type(),
                                    "O", self);
}   

static PyObject *Alias_RegionKey1(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getRegionKey1Type(),
                                    "O", self);
}   

static PyObject *Alias_Subgroup(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getSubgroupType(),
                                    "O", self);
}   

static PyObject *Alias_SubgroupKey0(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getSubgroupKey0Type(),
                                    "O", self);
}   

static PyObject *Alias_SubgroupKey1(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getSubgroupKey1Type(),
                                    "O", self);
}   

static PyObject *Alias_ProductData(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getProductDataType(),
                                    "O", self);
}   

static PyObject *Alias_ProductDataKey0(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getProductDataKey0Type(),
                                    "O", self);
}   

static PyObject *Alias_Account(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getAccountType(),
                                    "O", self);
}   

static PyObject *Alias_AccountKey0(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getAccountKey0Type(),
                                    "O", self);
}   

#ifndef DOMINO78
static PyObject *Alias_CheckSum(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getCheckSumType(),
                                    "O", self);
}   

static PyObject *Alias_CheckSumKey0(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getCheckSumKey0Type(),
                                    "O", self);
}   

static PyObject *Alias_CheckSumKey1(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getCheckSumKey1Type(),
                                    "O", self);
}   

static PyObject *Alias_Check(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getCheckType(),
                                    "O", self);
}   

static PyObject *Alias_CheckKey0(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getCheckKey0Type(),
                                    "O", self);
}   

static PyObject *Alias_CheckKey1(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getCheckKey1Type(),
                                    "O", self);
}   

static PyObject *Alias_CheckKey2(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getCheckKey2Type(),
                                    "O", self);
}   
#endif
static PyObject *Alias_DocumentView(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getDocumentViewType(),
                                    "O", self);
}   

static PyObject *Alias_DocumentViewKey0(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getDocumentViewKey0Type(),
                                    "O", self);
}   

static PyObject *Alias_DocumentViewKey1(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getDocumentViewKey1Type(),
                                    "O", self);
}   

static PyObject *Alias_Prowod(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getProwodType(),
                                    "O", self);
}   

static PyObject *Alias_ProwodKey0(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getProwodKey0Type(),
                                    "O", self);
}   

static PyObject *Alias_ProwodKey1(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getProwodKey1Type(),
                                    "O", self);
}   

static PyObject *Alias_ProwodKey2(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getProwodKey2Type(),
                                    "O", self);
}   

static PyObject *Alias_ProwodKey3(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getProwodKey3Type(),
                                    "O", self);
}   

static PyObject *Alias_ProwodKey4(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getProwodKey4Type(),
                                    "O", self);
}   

static PyObject *Alias_Product(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getProductType(),
                                    "O", self);
}   

static PyObject *Alias_ProductKey0(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getProductKey0Type(),
                                    "O", self);
}   

static PyObject *Alias_ProductKey1(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getProductKey1Type(),
                                    "O", self);
}   

static PyObject *Alias_ProductKey2(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getProductKey2Type(),
                                    "O", self);
}   

static PyObject *Alias_ProductKey3(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getProductKey3Type(),
                                    "O", self);
}   

static PyObject *Alias_ProductKey4(Alias* self)
{
    return PyObject_CallFunction((PyObject*)getProductKey4Type(),
                                    "O", self);
}   

static PyMethodDef Alias_methods[] = {
    {"get_name", (PyCFunction)Alias_get_name, METH_NOARGS,
     "do_alias_get_name"
    },
    {"get_host", (PyCFunction)Alias_get_host, METH_NOARGS,
     "do_alias_get_host"
    },
    {"utf8", (PyCFunction)Alias_utf8, METH_NOARGS,
     "do_alias_utf8"
    },
    {"transaction_start", (PyCFunction)Alias_start_tran, METH_NOARGS,// METH_VARARGS|METH_KEYWORDS,
     "transactiob start"
    },
    {"transaction_stop", (PyCFunction)Alias_end_tran, METH_NOARGS,// METH_VARARGS|METH_KEYWORDS,
     "transactiob stop"
    },
    {"transaction_abort", (PyCFunction)Alias_abort_tran, METH_NOARGS,// METH_VARARGS|METH_KEYWORDS,
     "transactiob abort"
    },
    {"connect", (PyCFunction)Alias_connect, METH_NOARGS,// METH_VARARGS|METH_KEYWORDS,
     "do_alias_open"
    },
    {"clear", (PyCFunction)Alias_clear, METH_NOARGS,// METH_VARARGS|METH_KEYWORDS,
     "do_alias_clear"
    },
    {"connected", (PyCFunction)Alias_connected, METH_NOARGS,// METH_VARARGS|METH_KEYWORDS,
     "do_alias_connected"
    },

    {"Partner", (PyCFunction)Alias_Partner,  METH_NOARGS, "Alias_Partner"},

    {"PartnerKey0", (PyCFunction)Alias_PartnerKey0,  METH_NOARGS, "Alias_PartnerKey0"},

    {"PartnerKey1", (PyCFunction)Alias_PartnerKey1,  METH_NOARGS, "Alias_PartnerKey1"},

    {"PartnerKey2", (PyCFunction)Alias_PartnerKey2,  METH_NOARGS, "Alias_PartnerKey2"},

    {"PartnerKey3", (PyCFunction)Alias_PartnerKey3,  METH_NOARGS, "Alias_PartnerKey3"},

    {"DocumentType", (PyCFunction)Alias_DocumentType,  METH_NOARGS, "Alias_DocumentType"},

    {"DocumentTypeKey0", (PyCFunction)Alias_DocumentTypeKey0,  METH_NOARGS, "Alias_DocumentTypeKey0"},

    {"Stock", (PyCFunction)Alias_Stock,  METH_NOARGS, "Alias_Stock"},

    {"StockKey0", (PyCFunction)Alias_StockKey0,  METH_NOARGS, "Alias_StockKey0"},

    {"StockKey1", (PyCFunction)Alias_StockKey1,  METH_NOARGS, "Alias_StockKey1"},

    {"StockKey2", (PyCFunction)Alias_StockKey2,  METH_NOARGS, "Alias_StockKey2"},

    {"StockKey3", (PyCFunction)Alias_StockKey3,  METH_NOARGS, "Alias_StockKey3"},

    {"Subaccount", (PyCFunction)Alias_Subaccount,  METH_NOARGS, "Alias_Subaccount"},

    {"SubaccountKey0", (PyCFunction)Alias_SubaccountKey0,  METH_NOARGS, "Alias_SubaccountKey0"},

    {"Saldo", (PyCFunction)Alias_Saldo,  METH_NOARGS, "Alias_Saldo"},

    {"SaldoKey0", (PyCFunction)Alias_SaldoKey0,  METH_NOARGS, "Alias_SaldoKey0"},

    {"ProductView", (PyCFunction)Alias_ProductView,  METH_NOARGS, "Alias_ProductView"},

    {"ProductViewKey0", (PyCFunction)Alias_ProductViewKey0,  METH_NOARGS, "Alias_ProductViewKey0"},

    {"ProductViewKey1", (PyCFunction)Alias_ProductViewKey1,  METH_NOARGS, "Alias_ProductViewKey1"},

    {"Group", (PyCFunction)Alias_Group,  METH_NOARGS, "Alias_Group"},

    {"GroupKey0", (PyCFunction)Alias_GroupKey0,  METH_NOARGS, "Alias_GroupKey0"},

    {"GroupKey1", (PyCFunction)Alias_GroupKey1,  METH_NOARGS, "Alias_GroupKey1"},

    {"User", (PyCFunction)Alias_User,  METH_NOARGS, "Alias_User"},

    {"UserKey0", (PyCFunction)Alias_UserKey0,  METH_NOARGS, "Alias_UserKey0"},

#ifndef DOMINO78
    {"Realization", (PyCFunction)Alias_Realization,  METH_NOARGS, "Alias_Realization"},

    {"RealizationKey0", (PyCFunction)Alias_RealizationKey0,  METH_NOARGS, "Alias_RealizationKey0"},

    {"RealizationKey1", (PyCFunction)Alias_RealizationKey1,  METH_NOARGS, "Alias_RealizationKey1"},

    {"RealizationKey2", (PyCFunction)Alias_RealizationKey2,  METH_NOARGS, "Alias_RealizationKey2"},

    {"RealizationKey3", (PyCFunction)Alias_RealizationKey3,  METH_NOARGS, "Alias_RealizationKey3"},

    {"RealizationKey4", (PyCFunction)Alias_RealizationKey4,  METH_NOARGS, "Alias_RealizationKey4"},

#endif
    {"Store", (PyCFunction)Alias_Store,  METH_NOARGS, "Alias_Store"},

    {"StoreKey0", (PyCFunction)Alias_StoreKey0,  METH_NOARGS, "Alias_StoreKey0"},

    {"Enum", (PyCFunction)Alias_Enum,  METH_NOARGS, "Alias_Enum"},

    {"EnumKey0", (PyCFunction)Alias_EnumKey0,  METH_NOARGS, "Alias_EnumKey0"},

    {"DocumentProw", (PyCFunction)Alias_DocumentProw,  METH_NOARGS, "Alias_DocumentProw"},

    {"DocumentProwKey0", (PyCFunction)Alias_DocumentProwKey0,  METH_NOARGS, "Alias_DocumentProwKey0"},

    {"Barcode", (PyCFunction)Alias_Barcode,  METH_NOARGS, "Alias_Barcode"},

    {"BarcodeKey0", (PyCFunction)Alias_BarcodeKey0,  METH_NOARGS, "Alias_BarcodeKey0"},

    {"BarcodeKey1", (PyCFunction)Alias_BarcodeKey1,  METH_NOARGS, "Alias_BarcodeKey1"},

    {"Prowod2", (PyCFunction)Alias_Prowod2,  METH_NOARGS, "Alias_Prowod2"},

    {"Prowod2Key0", (PyCFunction)Alias_Prowod2Key0,  METH_NOARGS, "Alias_Prowod2Key0"},

    {"Prowod2Key1", (PyCFunction)Alias_Prowod2Key1,  METH_NOARGS, "Alias_Prowod2Key1"},

    {"Prowod2Key2", (PyCFunction)Alias_Prowod2Key2,  METH_NOARGS, "Alias_Prowod2Key2"},

    {"Prowod2Key3", (PyCFunction)Alias_Prowod2Key3,  METH_NOARGS, "Alias_Prowod2Key3"},

    {"DocumentData", (PyCFunction)Alias_DocumentData,  METH_NOARGS, "Alias_DocumentData"},

    {"DocumentDataKey0", (PyCFunction)Alias_DocumentDataKey0,  METH_NOARGS, "Alias_DocumentDataKey0"},

    {"DocumentDataKey1", (PyCFunction)Alias_DocumentDataKey1,  METH_NOARGS, "Alias_DocumentDataKey1"},

    {"DocumentDataKey2", (PyCFunction)Alias_DocumentDataKey2,  METH_NOARGS, "Alias_DocumentDataKey2"},

#ifndef DOMINO78
    {"Shift", (PyCFunction)Alias_Shift,  METH_NOARGS, "Alias_Shift"},

    {"ShiftKey0", (PyCFunction)Alias_ShiftKey0,  METH_NOARGS, "Alias_ShiftKey0"},

    {"ShiftKey1", (PyCFunction)Alias_ShiftKey1,  METH_NOARGS, "Alias_ShiftKey1"},

    {"ShiftKey2", (PyCFunction)Alias_ShiftKey2,  METH_NOARGS, "Alias_ShiftKey2"},

    {"ShiftKey3", (PyCFunction)Alias_ShiftKey3,  METH_NOARGS, "Alias_ShiftKey3"},
#endif
    {"DocumentLink", (PyCFunction)Alias_DocumentLink,  METH_NOARGS, "Alias_DocumentLink"},

    {"DocumentLinkKey0", (PyCFunction)Alias_DocumentLinkKey0,  METH_NOARGS, "Alias_DocumentLinkKey0"},

    {"DocumentLinkKey1", (PyCFunction)Alias_DocumentLinkKey1,  METH_NOARGS, "Alias_DocumentLinkKey1"},

    {"PartnerData", (PyCFunction)Alias_PartnerData,  METH_NOARGS, "Alias_PartnerData"},

    {"PartnerDataKey0", (PyCFunction)Alias_PartnerDataKey0,  METH_NOARGS, "Alias_PartnerDataKey0"},

    {"PartnerDataKey1", (PyCFunction)Alias_PartnerDataKey1,  METH_NOARGS, "Alias_PartnerDataKey1"},

    {"PartnerDataKey2", (PyCFunction)Alias_PartnerDataKey2,  METH_NOARGS, "Alias_PartnerDataKey2"},

    {"Protocol", (PyCFunction)Alias_Protocol,  METH_NOARGS, "Alias_Protocol"},

    {"ProtocolKey0", (PyCFunction)Alias_ProtocolKey0,  METH_NOARGS, "Alias_ProtocolKey0"},

    {"ProtocolKey1", (PyCFunction)Alias_ProtocolKey1,  METH_NOARGS, "Alias_ProtocolKey1"},

    {"ProtocolKey2", (PyCFunction)Alias_ProtocolKey2,  METH_NOARGS, "Alias_ProtocolKey2"},

    {"Division", (PyCFunction)Alias_Division,  METH_NOARGS, "Alias_Division"},

    {"DivisionKey0", (PyCFunction)Alias_DivisionKey0,  METH_NOARGS, "Alias_DivisionKey0"},

    {"Document", (PyCFunction)Alias_Document,  METH_NOARGS, "Alias_Document"},

    {"DocumentKey0", (PyCFunction)Alias_DocumentKey0,  METH_NOARGS, "Alias_DocumentKey0"},

    {"DocumentKey1", (PyCFunction)Alias_DocumentKey1,  METH_NOARGS, "Alias_DocumentKey1"},

    {"DocumentKey2", (PyCFunction)Alias_DocumentKey2,  METH_NOARGS, "Alias_DocumentKey2"},

    {"DocumentKey3", (PyCFunction)Alias_DocumentKey3,  METH_NOARGS, "Alias_DocumentKey3"},

    {"DocumentKey4", (PyCFunction)Alias_DocumentKey4,  METH_NOARGS, "Alias_DocumentKey4"},

    {"DocumentOrder", (PyCFunction)Alias_DocumentOrder,  METH_NOARGS, "Alias_DocumentOrder"},

    {"DocumentOrderKey0", (PyCFunction)Alias_DocumentOrderKey0,  METH_NOARGS, "Alias_DocumentOrderKey0"},

    {"DocumentOrderKey1", (PyCFunction)Alias_DocumentOrderKey1,  METH_NOARGS, "Alias_DocumentOrderKey1"},

    {"DocumentOrderKey2", (PyCFunction)Alias_DocumentOrderKey2,  METH_NOARGS, "Alias_DocumentOrderKey2"},

    {"DocumentOrderKey3", (PyCFunction)Alias_DocumentOrderKey3,  METH_NOARGS, "Alias_DocumentOrderKey3"},

    {"DocumentOrderKey4", (PyCFunction)Alias_DocumentOrderKey4,  METH_NOARGS, "Alias_DocumentOrderKey4"},

    {"ProductLink", (PyCFunction)Alias_ProductLink,  METH_NOARGS, "Alias_ProductLink"},

    {"ProductLinkKey0", (PyCFunction)Alias_ProductLinkKey0,  METH_NOARGS, "Alias_ProductLinkKey0"},

    {"ProductLinkKey1", (PyCFunction)Alias_ProductLinkKey1,  METH_NOARGS, "Alias_ProductLinkKey1"},

    {"Class", (PyCFunction)Alias_Class,  METH_NOARGS, "Alias_Class"},

    {"ClassKey0", (PyCFunction)Alias_ClassKey0,  METH_NOARGS, "Alias_ClassKey0"},

    {"Region", (PyCFunction)Alias_Region,  METH_NOARGS, "Alias_Region"},

    {"RegionKey0", (PyCFunction)Alias_RegionKey0,  METH_NOARGS, "Alias_RegionKey0"},

    {"RegionKey1", (PyCFunction)Alias_RegionKey1,  METH_NOARGS, "Alias_RegionKey1"},

    {"Subgroup", (PyCFunction)Alias_Subgroup,  METH_NOARGS, "Alias_Subgroup"},

    {"SubgroupKey0", (PyCFunction)Alias_SubgroupKey0,  METH_NOARGS, "Alias_SubgroupKey0"},

    {"SubgroupKey1", (PyCFunction)Alias_SubgroupKey1,  METH_NOARGS, "Alias_SubgroupKey1"},

    {"ProductData", (PyCFunction)Alias_ProductData,  METH_NOARGS, "Alias_ProductData"},

    {"ProductDataKey0", (PyCFunction)Alias_ProductDataKey0,  METH_NOARGS, "Alias_ProductDataKey0"},

    {"Account", (PyCFunction)Alias_Account,  METH_NOARGS, "Alias_Account"},

    {"AccountKey0", (PyCFunction)Alias_AccountKey0,  METH_NOARGS, "Alias_AccountKey0"},

#ifndef DOMINO78
    {"CheckSum", (PyCFunction)Alias_CheckSum,  METH_NOARGS, "Alias_CheckSum"},

    {"CheckSumKey0", (PyCFunction)Alias_CheckSumKey0,  METH_NOARGS, "Alias_CheckSumKey0"},

    {"CheckSumKey1", (PyCFunction)Alias_CheckSumKey1,  METH_NOARGS, "Alias_CheckSumKey1"},

    {"Check", (PyCFunction)Alias_Check,  METH_NOARGS, "Alias_Check"},

    {"CheckKey0", (PyCFunction)Alias_CheckKey0,  METH_NOARGS, "Alias_CheckKey0"},

    {"CheckKey1", (PyCFunction)Alias_CheckKey1,  METH_NOARGS, "Alias_CheckKey1"},

    {"CheckKey2", (PyCFunction)Alias_CheckKey2,  METH_NOARGS, "Alias_CheckKey2"},

#endif
    {"DocumentView", (PyCFunction)Alias_DocumentView,  METH_NOARGS, "Alias_DocumentView"},

    {"DocumentViewKey0", (PyCFunction)Alias_DocumentViewKey0,  METH_NOARGS, "Alias_DocumentViewKey0"},

    {"DocumentViewKey1", (PyCFunction)Alias_DocumentViewKey1,  METH_NOARGS, "Alias_DocumentViewKey1"},

    {"Prowod", (PyCFunction)Alias_Prowod,  METH_NOARGS, "Alias_Prowod"},

    {"ProwodKey0", (PyCFunction)Alias_ProwodKey0,  METH_NOARGS, "Alias_ProwodKey0"},

    {"ProwodKey1", (PyCFunction)Alias_ProwodKey1,  METH_NOARGS, "Alias_ProwodKey1"},

    {"ProwodKey2", (PyCFunction)Alias_ProwodKey2,  METH_NOARGS, "Alias_ProwodKey2"},

    {"ProwodKey3", (PyCFunction)Alias_ProwodKey3,  METH_NOARGS, "Alias_ProwodKey3"},

    {"ProwodKey4", (PyCFunction)Alias_ProwodKey4,  METH_NOARGS, "Alias_ProwodKey4"},

    {"Product", (PyCFunction)Alias_Product,  METH_NOARGS, "Alias_Product"},

    {"ProductKey0", (PyCFunction)Alias_ProductKey0,  METH_NOARGS, "Alias_ProductKey0"},

    {"ProductKey1", (PyCFunction)Alias_ProductKey1,  METH_NOARGS, "Alias_ProductKey1"},

    {"ProductKey2", (PyCFunction)Alias_ProductKey2,  METH_NOARGS, "Alias_ProductKey2"},

    {"ProductKey3", (PyCFunction)Alias_ProductKey3,  METH_NOARGS, "Alias_ProductKey3"},

    {"ProductKey4", (PyCFunction)Alias_ProductKey4,  METH_NOARGS, "Alias_ProductKey4"},

    {NULL}  /* Sentinel */
};


static PyTypeObject AliasType = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.Alias",             /*tp_name*/
    sizeof(Alias),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Alias_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Alias objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    Alias_methods,             /* tp_methods */
    0,//Alias_members,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)Alias_init,      /* tp_init */
    0,                         /* tp_alloc */
    Alias_new,                 /* tp_new */
};
PyTypeObject *getAliasType()
{
    return &AliasType;
}

