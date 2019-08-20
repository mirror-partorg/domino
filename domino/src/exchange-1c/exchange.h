#ifndef EXCHANGE_H_INCLUDED
#define EXCHANGE_H_INCLUDED

#include <glib.h>
#include <gtk/gtk.h>
#include <libxml/parser.h>

#define OBJ_MY_ERROR(obj, ...) { gchar *v = g_strdup_printf(__VA_ARGS__); obj_error(obj, v); g_free(v);}
#define PROP_MY_ERROR(obj, ...) { gchar *v = g_strdup_printf(__VA_ARGS__); prop_error(obj, v); g_free(v);}
#define KEY_MY_ERROR(obj, ...) { gchar *v = g_strdup_printf(__VA_ARGS__); key_error(obj, v); g_free(v);}

typedef struct _ExchangeObject ExchangeObject;
typedef struct _ExchangeProperty ExchangeProperty;
typedef struct _ExchangeKey ExchangeKey;
typedef enum   _ExchangeType ExchangeType;
typedef enum   _ExchangeTypeKind ExchangeTypeKind;
typedef enum   _DoExchangeHandbook   DoExchangeHandbook;
typedef enum   _DoExchangeDocumentType   DoExchangeDocumentType;
typedef enum   _DoExchangeDocumentProductKind   DoExchangeDocumentProductKind;

enum _DoExchangeHandbook
{
    DO_EXCHANGE_HANDBOOK_PARTNER = 0,
    DO_EXCHANGE_HANDBOOK_SKLAD,
    //DO_EXCHANGE_HANDBOOK_CASHIER,
    DO_EXCHANGE_HANDBOOK_FIRM,
    DO_EXCHANGE_HANDBOOK_NO,
};

enum _DoExchangeDocumentProductKind
{
    DO_EXCHANGE_DOCUMENT_PRODUCT_KIND_IN = 0,
    DO_EXCHANGE_DOCUMENT_PRODUCT_KIND_MOVE,
    DO_EXCHANGE_DOCUMENT_PRODUCT_KIND_SURPLUS,
    DO_EXCHANGE_DOCUMENT_PRODUCT_KIND_RE_SORT,
    DO_EXCHANGE_DOCUMENT_PRODUCT_KIND_BACK,
    DO_EXCHANGE_DOCUMENT_PRODUCT_KIND_DEL,
    DO_EXCHANGE_DOCUMENT_PRODUCT_KIND_SALE,
    DO_EXCHANGE_DOCUMENT_PRODUCT_KIND_OTHER_OUTLAY,
    DO_EXCHANGE_DOCUMENT_PRODUCT_KIND_NO,
};

enum _ExchangeType {
    EXCHANGE_TYPE_HANDBOOK_CONTRACT = 0, //must be first
    EXCHANGE_TYPE_HANDBOOK_PARTNER,
    EXCHANGE_TYPE_HANDBOOK_ACCOUNT,
    EXCHANGE_TYPE_HANDBOOK_FIRM,
    EXCHANGE_TYPE_HANDBOOK_PRICE_TYPE,
    EXCHANGE_TYPE_HANDBOOK_UNIT,
    EXCHANGE_TYPE_HANDBOOK_PRODUCT_GROUP,
    EXCHANGE_TYPE_HANDBOOK_EI,
    EXCHANGE_TYPE_HANDBOOK_PRODUCT,
    EXCHANGE_TYPE_HANDBOOK_COST,
    EXCHANGE_TYPE_HANDBOOK_CONTACTS,
    EXCHANGE_TYPE_HANDBOOK_DECLARATION,
    EXCHANGE_TYPE_HANDBOOK_PRODUCT_SPECIFICATION,
    EXCHANGE_TYPE_HANDBOOK_PERSON_NATURAL,
    EXCHANGE_TYPE_HANDBOOK_UNIT_FIRM,
    EXCHANGE_TYPE_HANDBOOK_POST_FIRM,
    EXCHANGE_TYPE_HANDBOOK_MEMBER_FIRM,
    EXCHANGE_TYPE_HANDBOOK_VALUTA,
    EXCHANGE_TYPE_HANDBOOK_USER,
    EXCHANGE_TYPE_HANDBOOK_OTHER_PROFIT_OUTLAY,
    EXCHANGE_TYPE_HANDBOOK_KIND_MOVE_MONEY,
    EXCHANGE_TYPE_HANDBOOK_KIND_MONEY_PAY,
    EXCHANGE_TYPE_HANDBOOK_COUNTRY, //must be last

    EXCHANGE_TYPE_LIST_UNIT_KIND, //must be first
    EXCHANGE_TYPE_LIST_VAT_RATE,
    EXCHANGE_TYPE_LIST_EMPLOYMENT_KIND,
    EXCHANGE_TYPE_LIST_OPERATION_PRODUCT_IN,
    EXCHANGE_TYPE_LIST_OPERATION_PRODUCT_MOVE,
    EXCHANGE_TYPE_LIST_METHOD_CLOSE_ADVANCE,
    EXCHANGE_TYPE_LIST_KIND_CONTRACT,
    EXCHANGE_TYPE_LIST_SHOW_VUSN,
    EXCHANGE_TYPE_LIST_METHOD_VAT_IN_PRICE,
    EXCHANGE_TYPE_LIST_KIND_FOR_NALOG_COST,
    EXCHANGE_TYPE_LIST_OTHER_PROFIT_OUTLAY,
    EXCHANGE_TYPE_LIST_OPERATION_PRODUCT_DEL,
    EXCHANGE_TYPE_LIST_OPERATION_PRODUCT_BACK,
    EXCHANGE_TYPE_LIST_KIND_MOVE_MONEY,
    EXCHANGE_TYPE_LIST_KIND_MONEY_PAY,
    EXCHANGE_TYPE_LIST_OPERATION_PRODUCT_SALE,
    EXCHANGE_TYPE_LIST_PERSON_KIND, //must be last

    EXCHANGE_TYPE_ATTRIBUTE_ACCOUNT_DEFAULT,

    EXCHANGE_TYPE_ACCOUNT_DEFAULT,

    EXCHANGE_TYPE_CONST_BOOLEAN, //must be first
    EXCHANGE_TYPE_CONST_DATE,
    EXCHANGE_TYPE_CONST_NUMBER,
    EXCHANGE_TYPE_CONST_GUID,
    EXCHANGE_TYPE_CONST_STRING, //must be last

    EXCHANGE_TYPE_NO,
};
enum _ExchangeTypeKind {
    EXCHANGE_TYPE_KIND_HANDBOOK = 0,
    EXCHANGE_TYPE_KIND_LIST,
    EXCHANGE_TYPE_KIND_ATTRIBUTE,
    EXCHANGE_TYPE_KIND_ACCOUNT,
    EXCHANGE_TYPE_KIND_CONST,
    EXCHANGE_TYPE_KIND_NO,
};


struct _ExchangeObject {
    ExchangeType type;
    ExchangeTypeKind kind;
    ExchangeKey  *key;
    GSList *properties;
    gint line;
};

struct _ExchangeProperty {
    ExchangeType type;
    ExchangeTypeKind kind;
    gchar *name;
    GValue value;
    gboolean empty;
    gint line;
};

struct _ExchangeKey {
    gchar *guid;
    gchar *name;
    gchar *code;
    gboolean  group;
    ExchangeType owner_type;
    ExchangeKey *owner;
    gint line;
};

#ifdef __cplusplus
extern "C" {
#endif

gboolean parse_obj(xmlNode *node, ExchangeObject **obj);
gboolean parse_property(xmlNode *node, ExchangeTypeKind owner_kind, ExchangeProperty **property, ExchangeKey **key);
gboolean parse_key(xmlNode *node, ExchangeTypeKind owner_kind, ExchangeKey **key);
gboolean parse_value(xmlNode *node, ExchangeTypeKind owner_kind, ExchangeProperty *property);

ExchangeObject   *find_exchange_object_by_key(GSList *objs, ExchangeKey *key);
ExchangeObject   *find_exchange_object_by_guid(GSList *objs, const gchar *guid);
ExchangeObject   *find_exchange_object_by_const_name(GSList *objs, ExchangeType type, const gchar *code);
ExchangeObject   *find_exchange_object_by_value_string(GSList *objs, ExchangeType type, const gchar *name, const gchar *str);

GSList *find_exchange_objects_by_code(GSList *objs, ExchangeType type, const gchar *code);
GSList *find_exchange_objects_by_name(GSList *objs, ExchangeType type, const gchar *name);
GSList *find_exchange_objects_by_name_i(GSList *objs, ExchangeType type, const gchar *name);
GSList *find_exchange_objects_by_owner(GSList *objs, ExchangeType type, ExchangeObject *owner);

ExchangeObject   *find_exchange_list_by_value(GSList *objs, ExchangeType type, const gchar *value);

ExchangeProperty *exchange_object_get_property(ExchangeObject *obj, const gchar *name);
const gchar      *exchange_object_get_name(ExchangeObject *obj);
void              exchange_objects_free(GSList *objs);

const gchar      *exchange_type_get_name(ExchangeType type);
ExchangeTypeKind  exchange_type_get_kind(ExchangeType type);

void obj_error(ExchangeObject *obj, const gchar *msg);
void prop_error(ExchangeProperty *prop, const gchar *msg);
void key_error(ExchangeKey *key, const gchar *msg);

const gchar *get_partner_default_account(GSList *objs, ExchangeObject *partner, gboolean *many);


#ifdef __cplusplus
}
#endif


#endif // EXCHANGE_H_INCLUDED
