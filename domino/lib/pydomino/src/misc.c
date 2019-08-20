
#include "misc.h"
#include "../var/db.h"
#include "../var/util.h"

char *get_product_code(PyObject *product)
{
    char *code;
    if ( MyString_Check(product) ) {
        code = MyString_AsString(product);
    }
    else
    if ( Py_TYPE(product) == getProductType() ) {
        Product *obj=(Product *)product;
        code = do_text(obj->alias->alias, obj->priv->data.code);
    }
    else
    if ( Py_TYPE(product) == getProductKey0Type() ) {
        ProductKey0 *obj=(ProductKey0 *)product;
        code = do_text(obj->alias->alias, obj->priv->code);
    }
    else
    if ( Py_TYPE(product) == getProductKey4Type() ) {
        ProductKey4 *obj=(ProductKey4 *)product;
        code = do_text(obj->alias->alias, obj->priv->code);
    }
    else
    if ( Py_TYPE(product) == getBarcodeType() ) {
        Barcode *obj=(Barcode *)product;
        code = do_text(obj->alias->alias, obj->priv->data.code);
    }
    else
    if ( Py_TYPE(product) == getBarcodeKey1Type() ) {
        BarcodeKey1 *obj=(BarcodeKey1 *)product;
        code = do_text(obj->alias->alias, obj->priv->code);
    }
    else
    if ( Py_TYPE(product) == getDocumentOrderType() ) {
        DocumentOrder *obj=(DocumentOrder *)product;
        code = do_text(obj->alias->alias, obj->priv->data.code);
    }
    else
    if ( Py_TYPE(product) == getDocumentOrderKey2Type() ) {
        DocumentOrderKey2 *obj=(DocumentOrderKey2 *)product;
        code = do_text(obj->alias->alias, obj->priv->code);
    }
    else
    if ( Py_TYPE(product) == getDocumentOrderKey3Type() ) {
        DocumentOrderKey3 *obj=(DocumentOrderKey3 *)product;
        code = do_text(obj->alias->alias, obj->priv->code);
    }
    else
    if ( Py_TYPE(product) == getCheckType() ) {
        Check *obj=(Check *)product;
        code = do_text(obj->alias->alias, obj->priv->data.product_code);
    }
    else
    if ( Py_TYPE(product) == getCheckKey2Type() ) {
        CheckKey2 *obj=(CheckKey2 *)product;
        code = do_text(obj->alias->alias, obj->priv->product_code);
    }
    else {
        code = NULL;
    }
    return code;
}
char *get_barcode_code(PyObject *barcode)
{
    char *bar;
    if ( MyString_Check(barcode) ) {
        bar = MyString_AsString(barcode);
    }
    else
    if ( Py_TYPE(barcode) == getBarcodeType() ) {
        Barcode *obj=(Barcode *)barcode;
        bar = do_text(obj->alias->alias, obj->priv->data.barcode);
    }
    else
    if ( Py_TYPE(barcode) == getBarcodeKey0Type() ) {
        BarcodeKey0 *obj=(BarcodeKey0 *)barcode;
        bar = do_text(obj->alias->alias, obj->priv->barcode);
    }
    else
        bar = NULL;
    return bar;
}
char *get_store_code(PyObject *store)
{
    char *unit;
    if ( MyString_Check(store) ) {
        unit = MyString_AsString(store);
    }
    else
    if ( Py_TYPE(store) == getStoreType() ) {
        Store *obj=(Store *)store;
        unit = do_text(obj->alias->alias, obj->priv->data.code);
    }
    else
    if ( Py_TYPE(store) == getStockKey0Type() ) {
        StoreKey0 *obj=(StoreKey0 *)store;
        unit = do_text(obj->alias->alias, obj->priv->code);
    }
    else
    if ( Py_TYPE(store) == getDocumentType() ) {
        Document *obj=(Document *)store;
        unit = do_text(obj->alias->alias, obj->priv->data.sklad);
    }
    else
    if ( Py_TYPE(store) == getDocumentKey0Type() ) {
        DocumentKey0 *obj=(DocumentKey0 *)store;
        unit = do_text(obj->alias->alias, obj->priv->sklad);
    }
    else
    if ( Py_TYPE(store) == getDocumentKey3Type() ) {
        DocumentKey3 *obj=(DocumentKey3 *)store;
        unit = do_text(obj->alias->alias, obj->priv->sklad);
    }
    else
    if ( Py_TYPE(store) == getDocumentOrderType() ) {
        DocumentOrder *obj=(DocumentOrder *)store;
        unit = do_text(obj->alias->alias, obj->priv->data.sklad);
    }
    else
    if ( Py_TYPE(store) == getDocumentOrderKey0Type() ) {
        DocumentOrderKey2 *obj=(DocumentOrderKey2 *)store;
        unit = do_text(obj->alias->alias, obj->priv->sklad);
    }
    else
    if ( Py_TYPE(store) == getDocumentOrderKey1Type() ) {
        DocumentOrderKey2 *obj=(DocumentOrderKey2 *)store;
        unit = do_text(obj->alias->alias, obj->priv->sklad);
    }
    else
    if ( Py_TYPE(store) == getDocumentOrderKey2Type() ) {
        DocumentOrderKey2 *obj=(DocumentOrderKey2 *)store;
        unit = do_text(obj->alias->alias, obj->priv->sklad);
    }
    else
    if ( Py_TYPE(store) == getDocumentOrderKey4Type() ) {
        DocumentOrderKey2 *obj=(DocumentOrderKey2 *)store;
        unit = do_text(obj->alias->alias, obj->priv->sklad);
    }
    else {
        unit = NULL;
    }
    return unit;
}

static do_string_list_t *get_store_list(PyObject *stores)
{
    char *str = NULL;
    if ( MyString_Check(stores) )
        str = MyString_AsString(stores);
    else
    if ( Py_TYPE(stores) == getStoreType() )
        str = do_text(((Store*)stores)->alias->alias, ((Store*)stores)->priv->data.code);
    else
    if ( Py_TYPE(stores) == getStoreKey0Type() )
        str = do_text(((StoreKey0*)stores)->alias->alias, ((StoreKey0*)stores)->priv->code);
    else
    if ( Py_TYPE(stores) == getDocumentType() )
        str = do_text(((Document*)stores)->alias->alias, ((Document*)stores)->priv->data.sklad);
    else
    if ( Py_TYPE(stores) == getDocumentKey0Type() )
        str = do_text(((DocumentKey0*)stores)->alias->alias, ((DocumentKey0*)stores)->priv->sklad);
    else
    if ( Py_TYPE(stores) == getDocumentKey3Type() )
        str = do_text(((DocumentKey3*)stores)->alias->alias, ((DocumentKey3*)stores)->priv->sklad);
    else
    if ( Py_TYPE(stores) == getDocumentOrderType() )
        str = do_text(((DocumentOrder*)stores)->alias->alias, ((DocumentOrder*)stores)->priv->data.sklad);
    else
    if ( Py_TYPE(stores) == getDocumentOrderKey0Type() )
        str = do_text(((DocumentOrderKey0*)stores)->alias->alias, ((DocumentOrderKey0*)stores)->priv->sklad);
    else
    if ( Py_TYPE(stores) == getDocumentOrderKey1Type() )
        str = do_text(((DocumentOrderKey1*)stores)->alias->alias, ((DocumentOrderKey1*)stores)->priv->sklad);
    else
    if ( Py_TYPE(stores) == getDocumentOrderKey2Type() )
        str = do_text(((DocumentOrderKey2*)stores)->alias->alias, ((DocumentOrderKey2*)stores)->priv->sklad);
    else
    if ( Py_TYPE(stores) == getDocumentOrderKey4Type() )
        str = do_text(((DocumentOrderKey4*)stores)->alias->alias, ((DocumentOrderKey4*)stores)->priv->sklad);
    else
    if ( Py_TYPE(stores) == getProwod2Type() )
        str = do_text(((Prowod2*)stores)->alias->alias, ((Prowod2*)stores)->priv->data.sklad);
    else
    if ( Py_TYPE(stores) == getProwod2Key0Type() )
        str = do_text(((Prowod2Key0*)stores)->alias->alias, ((Prowod2Key0*)stores)->priv->sklad);
    else
    if ( Py_TYPE(stores) == getRealizationType() )
        str = do_text(((Realization*)stores)->alias->alias, ((Realization*)stores)->priv->data.sklad);
    else
    if ( Py_TYPE(stores) == getRealizationKey0Type() )
        str = do_text(((RealizationKey0*)stores)->alias->alias, ((RealizationKey0*)stores)->priv->sklad);
    else
    if ( Py_TYPE(stores) == getRealizationKey1Type() )
        str = do_text(((RealizationKey1*)stores)->alias->alias, ((RealizationKey1*)stores)->priv->sklad);
    else
    if ( Py_TYPE(stores) == getRealizationKey2Type() )
        str = do_text(((RealizationKey2*)stores)->alias->alias, ((RealizationKey2*)stores)->priv->sklad);
    else
    if ( Py_TYPE(stores) == getRealizationKey3Type() )
        str = do_text(((RealizationKey3*)stores)->alias->alias, ((RealizationKey3*)stores)->priv->sklad);
    else
    if ( Py_TYPE(stores) == getRealizationKey4Type() )
        str = do_text(((RealizationKey4*)stores)->alias->alias, ((RealizationKey4*)stores)->priv->sklad);
    else
    if ( Py_TYPE(stores) == getShiftType() )
        str = do_text(((Shift*)stores)->alias->alias, ((Shift*)stores)->priv->data.sklad);
    else
    if ( Py_TYPE(stores) == getShiftKey3Type() )
        str = do_text(((ShiftKey3*)stores)->alias->alias, ((ShiftKey3*)stores)->priv->sklad);

    if ( !str )
        return NULL;

    do_string_list_t *retval;
    retval = do_string_list_new_from_csv(str);
    do_free(str);
    return retval;
}
do_string_list_t *get_store_code_list(PyObject *stores)
{
    do_string_list_t *retval;
    retval = get_store_list(stores);
    if ( retval )
        return retval;

    if ( PyList_Check(stores) || PyTuple_Check(stores) ) {
        int i,j;
        int len;
        len = PyList_Check(stores) ? PyList_Size(stores) : PyTuple_Size(stores);
        retval = do_string_list_new();
        for ( i = 0; i < len; i++ ) {
            PyObject *value;
            do_string_list_t *item;
            value =  PyList_Check(stores) ? PyList_GetItem(stores, i) : PyTuple_GetItem(stores, i);
            item = get_store_list(value);
            if ( !item ) {
                do_string_list_free(retval);
                return NULL;
            }
            for ( j = 0; j < item->count; j++)
                do_string_list_add_alloc(retval, item->list[j]);
            do_string_list_free(item);
        }
        return retval;
    }
    return NULL;
}
char *get_partner_code(PyObject *partner, int *code)
{
    char *partner_region;
    if ( MyString_Check(partner) ) {
        char *value, *ch;
        value = MyString_AsString(partner);
        ch = strchr(value, '.');
        if ( !ch ) {
            do_free(value);
            return NULL;
        }
        *ch = '\0';
        partner_region = value;
        ch++;
        *code = atoi(ch);
    }
    else
    if ( Py_TYPE(partner) == getPartnerType() ) {
        Partner *obj=(Partner *)partner;
        partner_region = do_text(obj->alias->alias, obj->priv->data.g_code);
        *code = obj->priv->data.code;
    }
    else
    if ( Py_TYPE(partner) == getPartnerKey0Type() ) {
        PartnerKey0 *obj=(PartnerKey0 *)partner;
        partner_region = do_text(obj->alias->alias, obj->priv->g_code);
        *code = obj->priv->code;
    }
        return NULL;
    return partner_region;
}
