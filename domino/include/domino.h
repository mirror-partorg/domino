#ifndef DOMINO_H_INCLUDED
#define DOMINO_H_INCLUDED

#define DOMINO_CHARSET "CP866"
#define DOMINO_GUEST_NAME "domino"
#include <def.h>
#include <domino_db.h>
#include <dolib.h>
#include <stddef.h>

typedef enum {
    DOMINO_ROLE_HANDBOOK = 1 << 0,
    DOMINO_ROLE_OPERATOR = 1 << 1,
    DOMINO_ROLE_MAIN_OPERATOR = 1 << 2,
    DOMINO_ROLE_MANAGER = 1 << 3,
    DOMINO_ROLE_MAIN_MANAGER = 1 << 4,
    DOMINO_ROLE_BOOK = 1 << 5,
    DOMINO_ROLE_MAIN_BOOK = 1 << 6,
    DOMINO_ROLE_SUPERVISOR = 1 << 7,
} domino_role_t;

typedef struct _do_alias_t do_alias_t;
typedef struct _do_meta_alias_t do_meta_alias_t;

typedef enum {
    DOMINO_MARKED_UNMARK = 0,
    DOMINO_MARKED = 1,
    DOMINO_MARKED_AND_ACCEPT = 2,
    DOMINO_MARKED_AND_REJECT = 3,
} domino_marked_t;

#define DOMINO_MARKED_INTERVAL 7

typedef struct {
    do_alias_t *alias;
    struct tm date_b;
    struct tm date_e;
    do_string_list_t *sklads;
} do_alias_period_t;

#define DO_ERROR 0
#define DO_OK 1
#define DO_KEY_NOT_FOUND -1
#define DO_REJECT 2
#define DO_INUSE -2
#define DO_DUPLICATE_KEY_VALUE 3
#define BTI_BLOCK_SIZE 128
#define B_GET_KEY_ONLY 50
typedef enum {
    DO_GET_EQUAL = B_GET_EQUAL,
    DO_GET_NEXT = B_GET_NEXT,
    DO_GET_PREVIOUS = B_GET_PREVIOUS,
    DO_GET_GT = B_GET_GT,
    DO_GET_GE = B_GET_GE,
    DO_GET_LT = B_GET_LT,
    DO_GET_LE = B_GET_LE,
    DO_GET_FIRST = B_GET_FIRST,
    DO_GET_LAST = B_GET_LAST
} do_alias_oper_t;
typedef enum {
    DO_STEP_NEXT = B_STEP_NEXT,
    DO_STEP_PREVIOUS = B_STEP_PREVIOUS,
    DO_STEP_FIRST = B_STEP_FIRST,
    DO_STEP_LAST = B_STEP_LAST
} do_alias_step_t;
typedef struct {
    double rest;
    double sale;
    double price;
    struct tm life_time;
} do_rest_price_life_t;

typedef struct {
    char *code;
    double quant[2];
} do_rest_t;

typedef enum {
    DO_ACCEPT_DEFAULT = 1,
    DO_ACCEPT_SHORT = 2,
    DO_ACCEPT_AND_INSERT = 3
} do_accept_t;
typedef enum {
    DO_DEACCEPT_DEFAULT = 1,
    DO_DEACCEPT_SHORT = 2,
    DO_DEACCEPT_AND_DELETE = 3
} do_deaccept_t;
typedef struct _do_config_t do_config_t;
typedef enum {
    DO_REST_ACTION_NONE = 1,
    DO_REST_ACTION_IN = 2,
    DO_REST_ACTION_OUT = 3
} do_rest_action;

typedef struct _do_extended_t do_extended_t;
typedef struct _do_ret_list_t do_ret_list_t;
typedef int (*do_extended_break_func)(void *user_data);
typedef int (*do_extended_reject_func)(void *data, BTI_SINT recLen, BTI_LONG recPos, void *user_data);
typedef struct {
    char *dtype;
    char *dtype1;
    char *name;
    char *short_name;
    char *p_doc_name;
    struct {
        char *g_code;
        int code;
    } partner_default;
    int direction;
    struct {
        int number;
        do_rest_action action;
    } rest_action[4];
    do_list_t *price_accept_copy;
    do_list_t *price_select_copy;
    do_list_t *sums;
    int auto_parcel;
    int accept_by_line;
} do_doctype_t;
typedef enum {
    RZ_INFO_LINK_NO = 0,
    RZ_INFO_LINK_OK = 1,
    RZ_INFO_LINK_FAIL = 2,
} rz_info_link_type_t;
typedef struct PACK_STRUCT {
    char  *barcode;
    char  *mnn;
    char  *trade_name;
    char  *pack_form;
    int    pack_quant;
    char  *manufactor;
    double price;
    int    valuta;
    int    uniq_nx;
    int    unpack;
    struct {
        char  *barcode;
        rz_info_link_type_t type;
    } link;

    struct {
        long int time;
        char  *number;
    } order;

    struct {
        char *dtype;
        char *sklad;
        char *document;
        int   line;
    } key;
} rz_info_t;

typedef struct PACK_STRUCT {
    int price_reg;
    int aver_off;
    int round_up;
    int price_define;
    double price;
    do_list_t *scale;
    do_list_t *scale_max;
} markup_info_t;

typedef enum {
    DO_PROTOCOL_ACTION_INSERT = 0,
    DO_PROTOCOL_ACTION_INSERT_LINE,
    DO_PROTOCOL_ACTION_ACCEPT,
    DO_PROTOCOL_ACTION_DEACCEPT,
    DO_PROTOCOL_ACTION_CHANGE,
    DO_PROTOCOL_ACTION_DELETE,
    DO_PROTOCOL_ACTION_DELETE_LINE,
    DO_PROTOCOL_ACTION_CHANGE_KEY,
    DO_PROTOCOL_ACTION_CHANGE_LINE,
    DO_PROTOCOL_ACTION_CHANGE_LOCAL,
    DO_PROTOCOL_ACTION_LOGIN,
    DO_PROTOCOL_ACTION_LOGOUT,
    DO_PROTOCOL_ACTION_REPLIC,

    DO_PROTOCOL_ACTION_NONE
}do_protocol_action_t;
typedef enum {
    DO_PROTOCOL_OBJECT_PRODUCT = 0,
    DO_PROTOCOL_OBJECT_SKLAD,
    DO_PROTOCOL_OBJECT_REGION,
    DO_PROTOCOL_OBJECT_PARTNER,
    DO_PROTOCOL_OBJECT_CLASS,
    DO_PROTOCOL_OBJECT_GROUP,
    DO_PROTOCOL_OBJECT_SUBGROUP,
    DO_PROTOCOL_OBJECT_DOCUMENT,
    DO_PROTOCOL_OBJECT_CHECK,
    DO_PROTOCOL_OBJECT_SYSTEM,
    DO_PROTOCOL_OBJECT_REPLIC,
    DO_PROTOCOL_OBJECT_DOCUMENT_LINK,
    DO_PROTOCOL_OBJECT_PARTNER_DATA,
    DO_PROTOCOL_OBJECT_PRODUCT_DATA,
    DO_PROTOCOL_OBJECT_DOCUMENT_DATA,
    DO_PROTOCOL_OBJECT_DOCUMENT_ORDER,

    DO_PROTOCOL_OBJECT_OTHER,

    DO_PROTOCOL_OBJECT_NONE
}do_protocol_object_t;
typedef struct {
    BTI_CHAR division[DOMINO_FIELD_DEVISION];
	BTI_CHAR account[DOMINO_FIELD_ACCOUNT];
	BTI_CHAR subaccount[DOMINO_FIELD_SUBACCOUNT];
	BTI_CHAR p_g_code[DOMINO_FIELD_PARTNER_GCODE];
	BTI_LONG p_code;
	BTI_CHAR other[DOMINO_FIELD_OTHER];
	BTI_LONG date;
	double debit_sum[3];
	double credit_sum[3];
}do_balance_key_t;
typedef enum {
    DO_BALANCE_SIDE_DEBIT,
    DO_BALANCE_SIDE_CREDIT
}do_balance_side_t;
typedef struct {
    do_protocol_object_t object;
    union {
        product_key4_t product_key4;
        sklad_key0_t   sklad_key0;
        region_key0_t  region_key0;
        partner_key0_t partner_key0;
        class_key0_t   class_key0;
        group_key0_t   group_key0;
        subgroup_key0_t subgroup_key0;
        document_key0_t document_key0;
        document_order_key0_t document_order_key0;
#ifndef DOMINO78
        shift_key0_t  shift_key0;
#endif
        char other[1024];
    };
    do_protocol_action_t action;
    char action_context[1024];
}do_protocol_context_t;
typedef struct {
    do_doctype_t *dtype;
    double *sums;
    int protocol;
    char *pref;
} do_accept_param_t;

#define MAX_VALUE_SIZE 1024

#define DO_EXTENDED_COMP_EQUAL     1
#define DO_EXTENDED_COMP_GT        2
#define DO_EXTENDED_COMP_LT        3
#define DO_EXTENDED_COMP_NOT_EQUAL 4
#define DO_EXTENDED_COMP_GE        5
#define DO_EXTENDED_COMP_LE        6

#define DO_EXTENDED_TYPE_STRING B_STRING_TYPE
#define DO_EXTENDED_TYPE_INTEGER INTEGER_TYPE
#define DO_EXTENDED_TYPE_IEEE IEEE_TYPE
#define DO_EXTENDED_TYPE_DATE DATE_TYPE
#define DO_EXTENDED_TYPE_TIME TIME_TYPE
#define DO_EXTENDED_TYPE_DECIMAL DECIMAL_TYPE
#define DO_EXTENDED_TYPE_MONEY MONEY_TYPE
#define DO_EXTENDED_TYPE_LOGICAL LOGICAL_TYPE
#define DO_EXTENDED_TYPE_NUMERIC NUMERIC_TYPE
#define DO_EXTENDED_TYPE_BFLOAT BFLOAT_TYPE
#define DO_EXTENDED_TYPE_LSTRING LSTRING_TYPE
#define DO_EXTENDED_TYPE_ZSTRING ZSTRING_TYPE
#define DO_EXTENDED_TYPE_UNSIGNED_BINARY UNSIGNED_BINARY_TYPE
#define DO_EXTENDED_TYPE_AUTOINCREMENT AUTOINCREMENT_TYPE
#define DO_EXTENDED_TYPE_STS STS
#define DO_EXTENDED_TYPE_NUMERIC_SA NUMERIC_SA
#define DO_EXTENDED_TYPE_CURRENCY CURRENCY_TYPE
#define DO_EXTENDED_TYPE_TIMESTAMP TIMESTAMP_TYPE
#define DO_EXTENDED_TYPE_WSTRING WSTRING_TYPE
#define DO_EXTENDED_TYPE_WZSTRING WZSTRING_TYPE
#define DO_EXTENDED_TYPE_GUID GUID_TYPE
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define DO_EXTENDED_TYPE_NULL_INDICATOR NULL_INDICATOR_TYPE

#define DO_EXTENDED_CONNECTION_NO  0
#define DO_EXTENDED_CONNECTION_AND 1
#define DO_EXTENDED_CONNECTION_OR  2

typedef enum {
    DO_PARAM_PARAMETR_PRODUCT = 0,

    DO_PARAM_SKLAD_RETAIL_KIND,
    DO_PARAM_SKLAD_RETAIL_TYPE,

    DO_PARAM_PARTNER_REGION_DISCOUNT,
    DO_PARAM_PARTNER_REGION_CASH,
    DO_PARAM_PARTNER_REGION_CASHIER,

    DO_PARAM_DOCUMENT_TYPE_DISCOUNT,
    DO_PARAM_DOCUMENT_TYPE_PRODUCT_IN,
    DO_PARAM_DOCUMENT_TYPE_PRODUCT_OUT,
    DO_PARAM_DOCUMENT_TYPE_PRICE,
    DO_PARAM_DOCUMENT_TYPE_CRNTSALE,
    DO_PARAM_DOCUMENT_TYPE_ACCOUNT,
    DO_PARAM_DOCUMENT_TYPE_KEYBOARD,
    DO_PARAM_DOCUMENT_TYPE_ORDER1,
    DO_PARAM_DOCUMENT_TYPE_RZN,
    DO_PARAM_DOCUMENT_TYPE_RATE,
    DO_PARAM_DOCUMENT_TYPE_PKO,
    DO_PARAM_DOCUMENT_TYPE_RKO,

    DO_PARAM_DOCUMENT_OPERATION_PRODUCT_IN_INVOICE,
    DO_PARAM_DOCUMENT_OPERATION_ACCOUNT_INVOICE,
    DO_PARAM_DOCUMENT_OPERATION_CRNTSALE_DEFAULT,

    DO_PARAM_DOCUMENT_PARAMETR_COMMENT,
    DO_PARAM_DOCUMENT_PARAMETR_INVOICE,
    DO_PARAM_DOCUMENT_PARAMETR_INVOICE_DATE,

    DO_PARAM_DOCUMENT_PARAMETR_DISCOUNT_NAME,
    DO_PARAM_DOCUMENT_PARAMETR_DISCOUNT_KIND,
    DO_PARAM_DOCUMENT_PARAMETR_DISCOUNT_CALC,
    DO_PARAM_DOCUMENT_PARAMETR_DISCOUNT_SCALE,
    DO_PARAM_DOCUMENT_PARAMETR_DISCOUNT_PERC,
    DO_PARAM_DOCUMENT_PARAMETR_DISCOUNT_LIMIT_PRODUCT,
    DO_PARAM_DOCUMENT_PARAMETR_DISCOUNT_LIMIT_SUM,
    DO_PARAM_DOCUMENT_PARAMETR_DISCOUNT_LIMIT_PERIOD,
    DO_PARAM_DOCUMENT_PARAMETR_DISCOUNT_LIMIT_SUM_LOWER,
    DO_PARAM_DOCUMENT_PARAMETR_DISCOUNT_LIMIT_SUM_UPPER,
    DO_PARAM_DOCUMENT_PARAMETR_DISCOUNT_LIMIT_SKLAD,

    DO_PARAM_DOCUMENT_PARAMETR_KEYBOARD_KIND,
    DO_PARAM_DOCUMENT_PARAMETR_KEYBOARD_DESC,
    DO_PARAM_DOCUMENT_PARAMETR_KEYBOARD_PRODUCT,
    DO_PARAM_DOCUMENT_PARAMETR_KEYBOARD_COMMAND,
    DO_PARAM_DOCUMENT_PARAMETR_KEYBOARD_COMMAND_PARAMETERS,
    DO_PARAM_DOCUMENT_PARAMETR_KEYBOARD_FONT,

    DO_PARAM_DOCUMENT_PARAMETR_CHECK_SUM,
    DO_PARAM_DOCUMENT_PARAMETR_CHECK_TOTAL,
    DO_PARAM_DOCUMENT_PARAMETR_CASHIER,
    DO_PARAM_DOCUMENT_PARAMETR_INFO_PAY,
    DO_PARAM_DOCUMENT_PARAMETR_SUM_PURCHASE,
    DO_PARAM_DOCUMENT_PARAMETR_SUM_NDS_PURCHASE,


    DO_PARAM_DORDER_PARAMETR_CASHIER,
    DO_PARAM_DORDER_PARAMETR_OPERATION,
    DO_PARAM_DORDER_PARAMETR_CHECK,
    DO_PARAM_DORDER_PARAMETR_BARCODE,
    DO_PARAM_DORDER_PARAMETR_POSITION,
    DO_PARAM_DORDER_PARAMETR_MNN,
    DO_PARAM_DORDER_PARAMETR_TRADE_NAME,
    DO_PARAM_DORDER_PARAMETR_PACK_FORM,
    DO_PARAM_DORDER_PARAMETR_VALUTA_CODE,
    DO_PARAM_DORDER_PARAMETR_MANUFACTOR,
    DO_PARAM_DORDER_PARAMETR_SRC_FILENAME,
    DO_PARAM_DORDER_PARAMETR_NN,
    DO_PARAM_DORDER_PARAMETR_UNIQ_NX,
    DO_PARAM_DORDER_PARAMETR_UNPACK,
    DO_PARAM_DORDER_PARAMETR_NDS,
    DO_PARAM_DORDER_PARAMETR_NDS_SUPPLIER,
    DO_PARAM_DORDER_PARAMETR_TOTAL_PACK,
    DO_PARAM_DORDER_PARAMETR_SERIA,
    DO_PARAM_DORDER_PARAMETR_LIFE_TIME,
    DO_PARAM_DORDER_PARAMETR_FACT_RZ_PRICE,


    DO_PARAM_PROWOD_PARAMETR_DOCUMENT,
    DO_PARAM_PROWOD_PARAMETR_COMMENT,

    DO_PARAM_LINE_PARAMETR_DISCOUNT_KIND,
    DO_PARAM_LINE_PARAMETR_DISCOUNT_REDEFINE,

    DO_PARAM_PRICE_PURCHASE,
    DO_PARAM_PRICE_FACT,
    DO_PARAM_PRICE_RETAIL_DEFAULT,

    DO_PARAM_QUANT_DEFAULT,
    DO_PARAM_QUANT_CRNTSALE,
    DO_PARAM_QUANT_PRICE_PACK,
    DO_PARAM_QUANT_PRICE,

    DO_PARAM_SKLAD_PARAMETR_PRICE_RETAIL,
    DO_PARAM_SKLAD_PARAMETR_KIND,
    DO_PARAM_SKLAD_PARAMETR_KEYBOARD_WIDTH,
    DO_PARAM_SKLAD_PARAMETR_KEYBOARD_HEIGHT,
    DO_PARAM_SKLAD_PARAMETR_KEYBOARD_COL,
    DO_PARAM_SKLAD_PARAMETR_KEYBOARD_TEMPLATE,
    DO_PARAM_SKLAD_PARAMETR_PV_DEBIT_ACCOUNT,
    DO_PARAM_SKLAD_PARAMETR_PV_CREDIT_ACCOUNT,
    DO_PARAM_SKLAD_PARAMETR_PV_ACCOUNT,
    DO_PARAM_SKLAD_PARAMETR_PV_BANK_IN,
    DO_PARAM_SKLAD_PARAMETR_PV_BANK_IN_OSB,
    DO_PARAM_SKLAD_PARAMETR_PV_BANK_IN_BIK,
    DO_PARAM_SKLAD_PARAMETR_PV_BANK_OUT,
    DO_PARAM_SKLAD_PARAMETR_PV_BANK_OUT_OSB,
    DO_PARAM_SKLAD_PARAMETR_PV_BANK_OUT_BIK,
    DO_PARAM_SKLAD_PARAMETR_POST,


    DO_PARAM_PARTNER_PARAMETR_SKLAD1,
    DO_PARAM_PARTNER_PARAMETR_CASH_CONNECTION_STRING,
    DO_PARAM_PARTNER_PARAMETR_CASH_IP,
    DO_PARAM_PARTNER_PARAMETR_CASHIER_LOGIN,
    DO_PARAM_PARTNER_PARAMETR_CASHIER_PASSWORD,
    DO_PARAM_PARTNER_PARAMETR_CASHIER_ROLE,
    DO_PARAM_PARTNER_PARAMETR_CASHIER_FIO,
    DO_PARAM_PARTNER_PARAMETR_FIRM_DIRECTOR,
    DO_PARAM_PARTNER_PARAMETR_FIRM_MANAGER,
    DO_PARAM_PARTNER_PARAMETR_FIRM_BOOKKEEPER,
    DO_PARAM_PARTNER_PARAMETR_FIRM_CASHIER,
    DO_PARAM_PARTNER_PARAMETR_FIRM_NAME,
    DO_PARAM_PARTNER_PARAMETR_FIRM_INN_KPP,
    DO_PARAM_PARTNER_PARAMETR_FIRM_ADDRESS,
    DO_PARAM_PARTNER_PARAMETR_FIRM_INN,
    DO_PARAM_PARTNER_PARAMETR_FIRM_KPP,
    DO_PARAM_PARTNER_PARAMETR_FIRM_OKATO,
    DO_PARAM_PARTNER_PARAMETR_FIRM_FULL_NAME,
    DO_PARAM_PARTNER_PARAMETR_FIRM_OKPO,
    DO_PARAM_PARTNER_PARAMETR_FIRM_DIRECTOR_POST,
    DO_PARAM_PARTNER_PARAMETR_FIRM_DIRECTOR_FIO,
    DO_PARAM_PARTNER_PARAMETR_FIRM_BOOK_FIO,
    DO_PARAM_PARTNER_PARAMETR_FIRM_S_POST,
    DO_PARAM_PARTNER_PARAMETR_FIRM_S_FIO,

    DO_PARAM_PROTOCOL_ACTION_INSERT,
    DO_PARAM_PROTOCOL_ACTION_INSERT_LINE,
    DO_PARAM_PROTOCOL_ACTION_ACCEPT,
    DO_PARAM_PROTOCOL_ACTION_DEACCEPT,
    DO_PARAM_PROTOCOL_ACTION_CHANGE,
    DO_PARAM_PROTOCOL_ACTION_DELETE,
    DO_PARAM_PROTOCOL_ACTION_DELETE_LINE,
    DO_PARAM_PROTOCOL_ACTION_CHANGE_LINE,
    DO_PARAM_PROTOCOL_ACTION_CHANGE_KEY,
    DO_PARAM_PROTOCOL_ACTION_CHANGE_LOCAL,
    DO_PARAM_PROTOCOL_ACTION_LOGIN,
    DO_PARAM_PROTOCOL_ACTION_LOGOUT,
    DO_PARAM_PROTOCOL_ACTION_REPLIC,

    DO_PARAM_PROTOCOL_OBJECT_PRODUCT,
    DO_PARAM_PROTOCOL_OBJECT_SKLAD,
    DO_PARAM_PROTOCOL_OBJECT_REGION,
    DO_PARAM_PROTOCOL_OBJECT_PARTNER,
    DO_PARAM_PROTOCOL_OBJECT_CLASS,
    DO_PARAM_PROTOCOL_OBJECT_GROUP,
    DO_PARAM_PROTOCOL_OBJECT_SUBGROUP,
    DO_PARAM_PROTOCOL_OBJECT_DOCUMENT,
    DO_PARAM_PROTOCOL_OBJECT_CHECK,
    DO_PARAM_PROTOCOL_OBJECT_SYSTEM,
    DO_PARAM_PROTOCOL_OBJECT_REPLIC,
    DO_PARAM_PROTOCOL_OBJECT_DOCUMENT_LINK,
    DO_PARAM_PROTOCOL_OBJECT_PARTNER_DATA,
    DO_PARAM_PROTOCOL_OBJECT_PRODUCT_DATA,
    DO_PARAM_PROTOCOL_OBJECT_DOCUMENT_DATA,
    DO_PARAM_PROTOCOL_OBJECT_DOCUMENT_ORDER,
    DO_PARAM_PROTOCOL_OBJECT_OTHER,

    DO_PARAM_COMMON_SKLAD,

    DO_PARAM_COMMON_DATE_ACCEPT,
    DO_PARAM_COMMON_TIME_ACCEPT,

    DO_PARAM_PRODUCT_PARAMETR_LIFE_TIME,
    DO_PARAM_PRODUCT_PARAMETR_SERIA,
    DO_PARAM_PRODUCT_PARAMETR_NDS,
    DO_PARAM_PRODUCT_PARAMETR_NDS_SUPPLIER,
    DO_PARAM_PRODUCT_PARAMETR_CLASS,
    DO_PARAM_PRODUCT_PARAMETR_LIST,
    DO_PARAM_PRODUCT_PARAMETR_TEMPERATURE,
    DO_PARAM_PRODUCT_PARAMETR_IMPERATIVE,
    DO_PARAM_PRODUCT_PARAMETR_NO_QUANT_LIMIT,

    DO_PARAM_PRODUCT_BASE_PARCEL_SEPARATOR,

    //DO_PARAM_PRODUCT_CODE_FORMAT_DEPRECATE,
    //DO_PARAM_PRODUCT_BASE_CODE_FORMAT_DEPRECATE,
    DO_PARAM_PRODUCT_BASE_CODE_LENGTH_DEPRECATE,
    //DO_PARAM_PRODUCT_PARCEL_CODE_FORMAT_DEPRECATE,

    DP_PARAM_DOCUMENT_P_CASH_FORMAT,

    DO_PARAM_OTDEL_PARAMETR_FIRM,
    DO_PARAM_OTDEL_PARAMETR_NUMBER_FORMAT,


    DO_PARAM_CL_PARAMETR_DATE,
    DO_PARAM_CL_PARAMETR_DATE_B,
    DO_PARAM_CL_PARAMETR_TIME,
    DO_PARAM_CL_PARAMETR_CASH,
    DO_PARAM_CL_PARAMETR_SHIFT,
    DO_PARAM_CL_PARAMETR_CASHIER,

    DO_PARAM_ACCOUNT_PRODUCT_IN,
    DO_PARAM_ACCOUNT_REST_IN,
    DO_PARAM_ACCOUNT_SUM,
    DO_PARAM_ACCOUNT_SFF,

    DO_PARAM_VAR_PARAMETR,
    DO_PARAM_VAR_SUM,
    DO_PARAM_VAR_DOCUMENT_PARTNER,
    DO_PARAM_VAR_SKLAD_PARTNER,
    DO_PARAM_VAR_DOCUMENT,
    DO_PARAM_VAR_P_DOCUMENT_DATE_INT,
    DO_PARAM_VAR_COMMENT,
    DO_PARAM_VAR_P_DOCUMENT,
    DO_PARAM_VAR_SF_ANALIT,
    DO_PARAM_VAR_DOCUMENT_DATE,
    DO_PARAM_VAR_SF_NUMBER,
    DO_PARAM_VAR_SF_DATE,

    DO_PARAM_DPROWOD_PARAMETR_DEBIT_ACCOUNT,
    DO_PARAM_DPROWOD_PARAMETR_DEBIT_SUB_ACCOUNT,
    DO_PARAM_DPROWOD_PARAMETR_DEBIT_PARTNER,
    DO_PARAM_DPROWOD_PARAMETR_DEBIT_ANALIT,
    DO_PARAM_DPROWOD_PARAMETR_CREDIT_ACCOUNT,
    DO_PARAM_DPROWOD_PARAMETR_CREDIT_SUB_ACCOUNT,
    DO_PARAM_DPROWOD_PARAMETR_CREDIT_PARTNER,
    DO_PARAM_DPROWOD_PARAMETR_CREDIT_ANALIT,
    DO_PARAM_DPROWOD_PARAMETR_CREDIT_SUM_1,
    DO_PARAM_DPROWOD_PARAMETR_CREDIT_SUM_2,
    DO_PARAM_DPROWOD_PARAMETR_CREDIT_SUM_3,
    DO_PARAM_DPROWOD_PARAMETR_CREDIT_VED,
    DO_PARAM_DPROWOD_PARAMETR_CREDIT_COMMENT,
    DO_PARAM_DPROWOD_PARAMETR_CREDIT_DOCUMENT,
    DO_PARAM_DPROWOD_PARAMETR_CREDIT_DATE,
    DO_PARAM_DPROWOD_PARAMETR_CREDIT_CONDITION_1,
    DO_PARAM_DPROWOD_PARAMETR_CREDIT_CONDITION_2,

    DO_PARAM_NAME_SCALE,
    DO_PARAM_NAME_MAX_SCALE,
    DO_PARAM_NAME_KIND,
    DO_PARAM_NAME_KIND_MAX,
    DO_PARAM_NAME_SUPPLIER,
    DO_PARAM_NAME_RATE,
    DO_PARAM_NAME_MAX_RATE,
    DO_PARAM_NAME_GROUP,
    DO_PARAM_NAME_SUBGROUP,
    DO_PARAM_NAME_PRICE,
    DO_PARAM_NAME_VAT,
    DO_PARAM_NAME_ARM,
    DO_PARAM_NAME_MAIN,
    DO_PARAM_NAME_MAIN_SHORT,
    DO_PARAM_NAME_OPERATOR,
    DO_PARAM_NAME_MANAGER,
    DO_PARAM_NAME_BOOK,


    DO_PARAM_END // ALL_DP must be last item

} do_param_t;

#define DO_CONST_ACCEPT_ORDER_PARAM "ACPT"
#define DO_CONST_DEFAULT_DTYPE1 "1"
#define DO_CONST_DEFAULT_REGION "00"
#define DO_CONST_DEFAULT_PARTNER 1
#define DO_CONST_DEFAULT_DOCUMENT "000000"

#define DO_CONST_KEY_KIND_EMPTY 0
#define DO_CONST_KEY_KIND_BCODE 1
#define DO_CONST_KEY_KIND_BCODELIST 2
#define DO_CONST_KEY_KIND_COMMAND 3

#define DO_CONST_QUANT_REST 1
#define DO_CONST_QUANT_CRNTSALE 2
#define DO_CONST_QUANT_PRICE_FULL 3
#define DO_CONST_QUANT_PRICE 4
#define DO_CONST_QUANT_IGNORE 5

#define DO_CONST_PRICE_RETAIL 4
#define DO_CONST_PRICE_PURCHASE 2
#define DO_CONST_PRICE_CALC 11
#define DO_CONST_PRICE_PURCHASE_FACT 5
#define DO_CONST_PRICE_REGISTER 40
#define DO_CONST_PRICE_FACT 9
#define DO_CONST_PRICE_NDS_FACT 16
#define DO_CONST_PRICE_NDS_PURCHASE 15
#define DO_CONST_PRICE_NDS_PURCHASE_ 103
#define DO_CONST_PRICE_PURCHASE_ 102

#define DO_CONST_PROWOD_SUM_QUANT 0
#define DO_CONST_PROWOD_SUM_PRICE 1

#define DO_CONST_PRODUCT_CLASS_INDISPENSABLE_TO_LIFE 1
#define DO_CONST_PRODUCT_CLASS_DEFAULT 2


typedef struct _do_r3_var_t do_r3_var_t;
typedef do_r3_var_t *(*do_r3_var_get_value_func) (const char *name, void *user_data);

typedef struct _do_report_product_traffic_detail_t do_report_product_traffic_detail_t;


#ifdef __cplusplus
extern "C"
{
#endif

int         domino_init(const char *filename, int use_utf, do_param_t end);
int         domino_init_light(const char* local_unit, int use_utf, do_param_t end);

int         domino_check_configured();
int         domino_configure(const char* local_host, const char* main_host, const char* local_unit);
void        domino_finalize();
do_alias_t *domino_alias_new(const char* name);
do_alias_t *domino_alias_new_light(const char* local_unit);
void        domino_set_alias_name(const char* name);
void        domino_set_unit(const char* code);
int         domino_alias_set_store_number(do_alias_t *alias, int number, const char *hostname, const char *dbname);
int         domino_get_unit(do_alias_t *alias, sklad_rec_t *sklad, const char *code);
char       *domino_get_unit_prefix(do_alias_t *alias, const char *sklad);
int         domino_nretail_price(do_alias_t *alias, const char* sklad_code);
domino_role_t domino_get_role();
int         domino_check_role(int roles);
const char *domino_unit();
const char *domino_division();
int         domino_auth(const char *name, const char *password);
const char *domino_key_file();
do_string_list_t *domino_user_groups();
void        domino_set_debug_btr(int debug);


do_meta_alias_t   *domino_meta_alias_new();
do_sort_list_t    *domino_meta_alias_get_list_from_sklad(do_meta_alias_t *meta, const char *sklad, struct tm *date_b, struct tm *date_e);
do_sort_list_t    *domino_meta_alias_get_list_from_sklads(do_meta_alias_t *meta, do_string_list_t *sklads, struct tm *date_b, struct tm *date_e);
void               domino_meta_alias_free(do_meta_alias_t *meta);

int do_alias_auth(const char* dbConfDir, const char* aliasName, const char *name, const char *password, char **groups, char **rights, int *super,int utf);


//do_alias_t *do_alias_new(const char* dbConfDir, const char* aliasName, int breakOnError, const char *username, const char* password);
do_alias_t *do_alias_clone(do_alias_t *alias);
int        do_alias_utf8(do_alias_t *alias);
void       do_alias_set_wait_flag(do_alias_t *alias, int wait);
const char *do_alias_get_name(do_alias_t *alias);
const char *do_alias_get_host(do_alias_t *alias);
const char *do_alias_get_filename(do_alias_t *alias, file_db_t id);
void       do_alias_free(do_alias_t *alias);
void       do_alias_set_struct_location(do_alias_t *alias, const char *path);
int        do_alias_open(do_alias_t *alias, int openAll, ...);
int        do_alias_open_manual(do_alias_t *alias, ...);
int        do_alias_conected(do_alias_t *alias);
int        do_alias_close(do_alias_t *alias);
void       do_alias_set_dublicate_allow(do_alias_t *alias, int allow);
int        do_alias_get_dublicate_allow(do_alias_t *alias);


int        do_alias_db_number_records(do_alias_t *alias, file_db_t id);

int        do_alias_tran_begin(do_alias_t *alias);
int        do_alias_tran_end(do_alias_t *alias);
int        do_alias_tran_abort(do_alias_t *alias);
#ifdef USE_BTI
int do_alias_btr(do_alias_t *alias, BTI_WORD operation, file_db_t id, BTI_VOID_PTR dataBuffer, BTI_WORD_PTR dataLength, BTI_VOID_PTR keyBuffer, BTI_SINT keyNumber);
#endif

int        do_alias_text_check_set(do_alias_t *alias, const char* src);
char      *do_alias_text_(do_alias_t *alias, void* src, size_t size);
#define do_text(alias,src) do_alias_text_(alias,src,sizeof(src))
void      *do_alias_text_set_(do_alias_t *alias, void* dst, const char* src, size_t size);
#define do_text_set(alias,dst,src) do_alias_text_set_(alias,dst,src,sizeof(dst))
int        do_text_is_empty_(char* src, size_t size);
#define do_text_is_empty(src) do_text_is_empty_(src,sizeof(src))
void  *do_text_set_empty_(void* src, size_t size);
#define do_text_set_empty(src) do_text_set_empty_(src,sizeof(src))
time_t do_date(BTI_LONG date, struct tm *tm);
time_t do_time(BTI_LONG time, struct tm *tm);
int do_date_set(BTI_LONG *date, const struct tm tm);
int do_date_cmp(struct tm *tm1, struct tm *tm2);
int do_time_cmp(struct tm *tm1, struct tm *tm2);
int do_datetime_cmp(struct tm *tm1, struct tm *tm2);


int do_time_set(BTI_LONG *time, const struct tm tm);
char *do_product_create_parcel(do_alias_t *alias, const char* base_code, const char *pref);
char *do_product_get_name(do_alias_t *alias, const char* code);
int   do_document_get_last_line(do_alias_t *alias, document_key0_t *document_key0);

int do_struct_file_get0(do_alias_t *alias, struct_file_rec_t *rec, struct_file_key0_t *key, do_alias_oper_t operation);
int do_struct_file_get1(do_alias_t *alias, struct_file_rec_t *rec, struct_file_key1_t *key, do_alias_oper_t operation);
int do_struct_file_key0(do_alias_t *alias, struct_file_key0_t *key, do_alias_oper_t operation);
int do_struct_file_key1(do_alias_t *alias, struct_file_key1_t *key, do_alias_oper_t operation);
int do_struct_file_insert(do_alias_t *alias, struct_file_rec_t *rec);
int do_struct_file_update(do_alias_t *alias, struct_file_rec_t *rec);
int do_struct_file_delete(do_alias_t *alias, struct_file_rec_t *rec);

int do_product_get_code_len(char *code, int code_len);

int do_document_get0(do_alias_t *alias, document_rec_t *rec, document_key0_t *key, do_alias_oper_t operation);
int do_document_key0(do_alias_t *alias, document_key0_t *key, do_alias_oper_t operation);
int do_document_get1(do_alias_t *alias, document_rec_t *rec, document_key1_t *key, do_alias_oper_t operation);
int do_document_key1(do_alias_t *alias, document_key1_t *key, do_alias_oper_t operation);
int do_document_get2(do_alias_t *alias, document_rec_t *rec, document_key2_t *key, do_alias_oper_t operation);
int do_document_key2(do_alias_t *alias, document_key2_t *key, do_alias_oper_t operation);
int do_document_get3(do_alias_t *alias, document_rec_t *rec, document_key3_t *key, do_alias_oper_t operation);
int do_document_key3(do_alias_t *alias, document_key3_t *key, do_alias_oper_t operation);
#ifdef DOMINO78
int do_document_get4(do_alias_t *alias, document_rec_t *rec, document_key4_t *key, do_alias_oper_t operation);
int do_document_key4(do_alias_t *alias, document_key4_t *key, do_alias_oper_t operation);
#endif
int do_document_order_get0(do_alias_t *alias, document_order_rec_t *rec, document_order_key0_t *key, do_alias_oper_t operation);
int do_document_order_key0(do_alias_t *alias, document_order_key0_t *key, do_alias_oper_t operation);
int do_document_order_get1(do_alias_t *alias, document_order_rec_t *rec, document_order_key1_t *key, do_alias_oper_t operation);
int do_document_order_key1(do_alias_t *alias, document_order_key1_t *key, do_alias_oper_t operation);
int do_document_order_get2(do_alias_t *alias, document_order_rec_t *rec, document_order_key2_t *key, do_alias_oper_t operation);
int do_document_order_key2(do_alias_t *alias, document_order_key2_t *key, do_alias_oper_t operation);
int do_document_order_get3(do_alias_t *alias, document_order_rec_t *rec, document_order_key3_t *key, do_alias_oper_t operation);
int do_document_order_key3(do_alias_t *alias, document_order_key3_t *key, do_alias_oper_t operation);
int do_document_order_get4(do_alias_t *alias, document_order_rec_t *rec, document_order_key4_t *key, do_alias_oper_t operation);
int do_document_order_key4(do_alias_t *alias, document_order_key4_t *key, do_alias_oper_t operation);
int do_document_link_get0(do_alias_t *alias, document_link_rec_t *rec, document_link_key0_t *key, do_alias_oper_t operation);
int do_document_link_key0(do_alias_t *alias, document_link_key0_t *key, do_alias_oper_t operation);
int do_document_link_get1(do_alias_t *alias, document_link_rec_t *rec, document_link_key1_t *key, do_alias_oper_t operation);
int do_document_link_key1(do_alias_t *alias, document_link_key1_t *key, do_alias_oper_t operation);
int do_enum_get0(do_alias_t *alias, enum_rec_t *rec, enum_key0_t *key, do_alias_oper_t operation);
int do_enum_key0(do_alias_t *alias, enum_key0_t *key, do_alias_oper_t operation);
int do_document_view_get0(do_alias_t *alias, document_view_rec_t *rec, document_view_key0_t *key, do_alias_oper_t operation);
int do_document_view_key0(do_alias_t *alias, document_view_key0_t *key, do_alias_oper_t operation);
int do_document_view_get1(do_alias_t *alias, document_view_rec_t *rec, document_view_key1_t *key, do_alias_oper_t operation);
int do_document_view_key1(do_alias_t *alias, document_view_key1_t *key, do_alias_oper_t operation);
int do_document_data_get0(do_alias_t *alias, document_data_rec_t *rec, document_data_key0_t *key, do_alias_oper_t operation);
int do_document_data_key0(do_alias_t *alias, document_data_key0_t *key, do_alias_oper_t operation);
#ifdef DOMINO78
int do_document_data_get1(do_alias_t *alias, document_data_rec_t *rec, document_data_key1_t *key, do_alias_oper_t operation);
int do_document_data_key1(do_alias_t *alias, document_data_key1_t *key, do_alias_oper_t operation);
int do_document_data_get2(do_alias_t *alias, document_data_rec_t *rec, document_data_key2_t *key, do_alias_oper_t operation);
int do_document_data_key2(do_alias_t *alias, document_data_key2_t *key, do_alias_oper_t operation);
#endif
int do_product_get0(do_alias_t *alias, product_rec_t *rec, product_key0_t *key, do_alias_oper_t operation);
int do_product_key0(do_alias_t *alias, product_key0_t *key, do_alias_oper_t operation);
int do_product_get1(do_alias_t *alias, product_rec_t *rec, product_key1_t *key, do_alias_oper_t operation);
int do_product_key1(do_alias_t *alias, product_key1_t *key, do_alias_oper_t operation);
int do_product_get2(do_alias_t *alias, product_rec_t *rec, product_key2_t *key, do_alias_oper_t operation);
int do_product_key2(do_alias_t *alias, product_key2_t *key, do_alias_oper_t operation);
int do_product_get3(do_alias_t *alias, product_rec_t *rec, product_key3_t *key, do_alias_oper_t operation);
int do_product_key3(do_alias_t *alias, product_key3_t *key, do_alias_oper_t operation);
int do_product_get4(do_alias_t *alias, product_rec_t *rec, product_key4_t *key, do_alias_oper_t operation);
int do_product_key4(do_alias_t *alias, product_key4_t *key, do_alias_oper_t operation);
int do_product_link_get0(do_alias_t *alias, product_link_rec_t *rec, product_link_key0_t *key, do_alias_oper_t operation);
int do_product_link_key0(do_alias_t *alias, product_link_key0_t *key, do_alias_oper_t operation);
int do_product_link_get1(do_alias_t *alias, product_link_rec_t *rec, product_link_key1_t *key, do_alias_oper_t operation);
int do_product_link_key1(do_alias_t *alias, product_link_key1_t *key, do_alias_oper_t operation);
int do_product_view_get0(do_alias_t *alias, product_view_rec_t *rec, product_view_key0_t *key, do_alias_oper_t operation);
int do_product_view_key0(do_alias_t *alias, product_view_key0_t *key, do_alias_oper_t operation);
int do_product_view_get1(do_alias_t *alias, product_view_rec_t *rec, product_view_key1_t *key, do_alias_oper_t operation);
int do_product_view_key1(do_alias_t *alias, product_view_key1_t *key, do_alias_oper_t operation);
int do_product_view_number_records(do_alias_t *alias);
int do_product_data_get0(do_alias_t *alias, product_data_rec_t *rec, product_data_key0_t *key, do_alias_oper_t operation);
int do_product_data_key0(do_alias_t *alias, product_data_key0_t *key, do_alias_oper_t operation);
int do_product_data_get1(do_alias_t *alias, product_data_rec_t *rec, product_data_key1_t *key, do_alias_oper_t operation);
int do_product_data_key1(do_alias_t *alias, product_data_key1_t *key, do_alias_oper_t operation);
int do_product_data_get2(do_alias_t *alias, product_data_rec_t *rec, product_data_key2_t *key, do_alias_oper_t operation);
int do_product_data_key2(do_alias_t *alias, product_data_key2_t *key, do_alias_oper_t operation);
int do_barcode_get0(do_alias_t *alias, barcode_rec_t *rec, barcode_key0_t *key, do_alias_oper_t operation);
int do_barcode_key0(do_alias_t *alias, barcode_key0_t *key, do_alias_oper_t operation);
int do_barcode_get1(do_alias_t *alias, barcode_rec_t *rec, barcode_key1_t *key, do_alias_oper_t operation);
int do_barcode_key1(do_alias_t *alias, barcode_key1_t *key, do_alias_oper_t operation);
int do_barcode_number_records(do_alias_t *alias);
double do_barcode_sk(do_alias_t *alias, barcode_rec_t *rec);
int do_account_get0(do_alias_t *alias, account_rec_t *rec, account_key0_t *key, do_alias_oper_t operation);
int do_account_key0(do_alias_t *alias, account_key0_t *key, do_alias_oper_t operation);
int do_subaccount_get0(do_alias_t *alias, subaccount_rec_t *rec, subaccount_key0_t *key, do_alias_oper_t operation);
int do_subaccount_key0(do_alias_t *alias, subaccount_key0_t *key, do_alias_oper_t operation);
int do_class_get0(do_alias_t *alias, class_rec_t *rec, class_key0_t *key, do_alias_oper_t operation);
int do_class_key0(do_alias_t *alias, class_key0_t *key, do_alias_oper_t operation);
int do_class_number_records(do_alias_t *alias);
int do_group_get0(do_alias_t *alias, group_rec_t *rec, group_key0_t *key, do_alias_oper_t operation);
int do_group_key0(do_alias_t *alias, group_key0_t *key, do_alias_oper_t operation);
int do_group_get1(do_alias_t *alias, group_rec_t *rec, group_key1_t *key, do_alias_oper_t operation);
int do_group_key1(do_alias_t *alias, group_key1_t *key, do_alias_oper_t operation);
int do_group_number_records(do_alias_t *alias);
int do_subgroup_get0(do_alias_t *alias, subgroup_rec_t *rec, subgroup_key0_t *key, do_alias_oper_t operation);
int do_subgroup_key0(do_alias_t *alias, subgroup_key0_t *key, do_alias_oper_t operation);
int do_subgroup_get1(do_alias_t *alias, subgroup_rec_t *rec, subgroup_key1_t *key, do_alias_oper_t operation);
int do_subgroup_key1(do_alias_t *alias, subgroup_key1_t *key, do_alias_oper_t operation);
int do_subgroup_number_records(do_alias_t *alias);
int do_region_get0(do_alias_t *alias, region_rec_t *rec, region_key0_t *key, do_alias_oper_t operation);
int do_region_key0(do_alias_t *alias, region_key0_t *key, do_alias_oper_t operation);
int do_region_get1(do_alias_t *alias, region_rec_t *rec, region_key1_t *key, do_alias_oper_t operation);
int do_region_key1(do_alias_t *alias, region_key1_t *key, do_alias_oper_t operation);
int do_region_number_records(do_alias_t *alias);
int do_otdel_get0(do_alias_t *alias, otdel_rec_t *rec, otdel_key0_t *key, do_alias_oper_t operation);
int do_otdel_key0(do_alias_t *alias, otdel_key0_t *key, do_alias_oper_t operation);
int do_partner_get0(do_alias_t *alias, partner_rec_t *rec, partner_key0_t *key, do_alias_oper_t operation);
int do_partner_key0(do_alias_t *alias, partner_key0_t *key, do_alias_oper_t operation);
int do_partner_get1(do_alias_t *alias, partner_rec_t *rec, partner_key1_t *key, do_alias_oper_t operation);
int do_partner_key1(do_alias_t *alias, partner_key1_t *key, do_alias_oper_t operation);
int do_partner_get2(do_alias_t *alias, partner_rec_t *rec, partner_key2_t *key, do_alias_oper_t operation);
int do_partner_key2(do_alias_t *alias, partner_key2_t *key, do_alias_oper_t operation);
int do_partner_get3(do_alias_t *alias, partner_rec_t *rec, partner_key3_t *key, do_alias_oper_t operation);
int do_partner_key3(do_alias_t *alias, partner_key3_t *key, do_alias_oper_t operation);
int do_partner_number_records(do_alias_t *alias);
#ifdef USE_DB_PARNER_VIEW
int do_partner_view_get0(do_alias_t *alias, partner_view_rec_t *rec, partner_view_key0_t *key, do_alias_oper_t operation);
int do_partner_view_key0(do_alias_t *alias, partner_view_key0_t *key, do_alias_oper_t operation);
int do_partner_view_get1(do_alias_t *alias, partner_view_rec_t *rec, partner_view_key1_t *key, do_alias_oper_t operation);
int do_partner_view_key1(do_alias_t *alias, partner_view_key1_t *key, do_alias_oper_t operation);
#endif
int do_partner_data_get0(do_alias_t *alias, partner_data_rec_t *rec, partner_data_key0_t *key, do_alias_oper_t operation);
int do_partner_data_key0(do_alias_t *alias, partner_data_key0_t *key, do_alias_oper_t operation);
int do_partner_data_get1(do_alias_t *alias, partner_data_rec_t *rec, partner_data_key1_t *key, do_alias_oper_t operation);
int do_partner_data_key1(do_alias_t *alias, partner_data_key1_t *key, do_alias_oper_t operation);
#ifdef DOMINO78
int do_partner_data_get2(do_alias_t *alias, partner_data_rec_t *rec, partner_data_key2_t *key, do_alias_oper_t operation);
int do_partner_data_key2(do_alias_t *alias, partner_data_key2_t *key, do_alias_oper_t operation);
#endif

int do_sklad_get0(do_alias_t *alias, sklad_rec_t *rec, sklad_key0_t *key, do_alias_oper_t operation);
int do_sklad_key0(do_alias_t *alias, sklad_key0_t *key, do_alias_oper_t operation);
int do_document_prow_get0(do_alias_t *alias, document_prow_rec_t *rec, document_prow_key0_t *key, do_alias_oper_t operation);
int do_document_prow_key0(do_alias_t *alias, document_prow_key0_t *key, do_alias_oper_t operation);
int do_document_type_get0(do_alias_t *alias, document_type_rec_t *rec, document_type_key0_t *key, do_alias_oper_t operation);
int do_document_type_key0(do_alias_t *alias, document_type_key0_t *key, do_alias_oper_t operation);
int do_stock_get0(do_alias_t *alias, stock_rec_t *rec, stock_key0_t *key, do_alias_oper_t operation);
int do_stock_key0(do_alias_t *alias, stock_key0_t *key, do_alias_oper_t operation);
int do_stock_get1(do_alias_t *alias, stock_rec_t *rec, stock_key1_t *key, do_alias_oper_t operation);
int do_stock_key1(do_alias_t *alias, stock_key1_t *key, do_alias_oper_t operation);
#ifdef DOMINO78
int do_stock_get2(do_alias_t *alias, stock_rec_t *rec, stock_key2_t *key, do_alias_oper_t operation);
int do_stock_key2(do_alias_t *alias, stock_key2_t *key, do_alias_oper_t operation);
int do_stock_get3(do_alias_t *alias, stock_rec_t *rec, stock_key3_t *key, do_alias_oper_t operation);
int do_stock_key3(do_alias_t *alias, stock_key3_t *key, do_alias_oper_t operation);
#endif
int do_prowod_get0(do_alias_t *alias, prowod_rec_t *rec, prowod_key0_t *key, do_alias_oper_t operation);
int do_prowod_key0(do_alias_t *alias, prowod_key0_t *key, do_alias_oper_t operation);
int do_prowod_get1(do_alias_t *alias, prowod_rec_t *rec, prowod_key1_t *key, do_alias_oper_t operation);
int do_prowod_key1(do_alias_t *alias, prowod_key1_t *key, do_alias_oper_t operation);
int do_prowod_get2(do_alias_t *alias, prowod_rec_t *rec, prowod_key2_t *key, do_alias_oper_t operation);
int do_prowod_key2(do_alias_t *alias, prowod_key2_t *key, do_alias_oper_t operation);
int do_prowod_get3(do_alias_t *alias, prowod_rec_t *rec, prowod_key3_t *key, do_alias_oper_t operation);
int do_prowod_key3(do_alias_t *alias, prowod_key3_t *key, do_alias_oper_t operation);
int do_prowod_get4(do_alias_t *alias, prowod_rec_t *rec, prowod_key4_t *key, do_alias_oper_t operation);
int do_prowod_key4(do_alias_t *alias, prowod_key4_t *key, do_alias_oper_t operation);
int do_prowod2_get0(do_alias_t *alias, prowod2_rec_t *rec, prowod2_key0_t *key, do_alias_oper_t operation);
int do_prowod2_key0(do_alias_t *alias, prowod2_key0_t *key, do_alias_oper_t operation);
int do_prowod2_get1(do_alias_t *alias, prowod2_rec_t *rec, prowod2_key1_t *key, do_alias_oper_t operation);
int do_prowod2_key1(do_alias_t *alias, prowod2_key1_t *key, do_alias_oper_t operation);
int do_prowod2_get2(do_alias_t *alias, prowod2_rec_t *rec, prowod2_key2_t *key, do_alias_oper_t operation);
int do_prowod2_key2(do_alias_t *alias, prowod2_key2_t *key, do_alias_oper_t operation);
int do_prowod2_get3(do_alias_t *alias, prowod2_rec_t *rec, prowod2_key3_t *key, do_alias_oper_t operation);
int do_prowod2_key3(do_alias_t *alias, prowod2_key3_t *key, do_alias_oper_t operation);
int do_saldo_get0(do_alias_t *alias, saldo_rec_t *rec, saldo_key0_t *key, do_alias_oper_t operation);
int do_saldo_key0(do_alias_t *alias, saldo_key0_t *key, do_alias_oper_t operation);
int do_protocol_step(do_alias_t *alias, protocol_rec_t *rec,  do_alias_step_t operation);
int do_protocol_get0(do_alias_t *alias, protocol_rec_t *rec, protocol_key0_t *key, do_alias_oper_t operation);
int do_protocol_key0(do_alias_t *alias, protocol_key0_t *key, do_alias_oper_t operation);
int do_protocol_get1(do_alias_t *alias, protocol_rec_t *rec, protocol_key1_t *key, do_alias_oper_t operation);
int do_protocol_key1(do_alias_t *alias, protocol_key1_t *key, do_alias_oper_t operation);
int do_protocol_get2(do_alias_t *alias, protocol_rec_t *rec, protocol_key2_t *key, do_alias_oper_t operation);
int do_protocol_key2(do_alias_t *alias, protocol_key2_t *key, do_alias_oper_t operation);
int do_user_get0(do_alias_t *alias, user_rec_t *rec, user_key0_t *key, do_alias_oper_t operation);
int do_user_key0(do_alias_t *alias, user_key0_t *key, do_alias_oper_t operation);

#ifndef DOMINO78
int do_shift_get0(do_alias_t *alias, shift_rec_t *rec, shift_key0_t *key, do_alias_oper_t operation);
int do_shift_key0(do_alias_t *alias, shift_key0_t *key, do_alias_oper_t operation);
int do_shift_get1(do_alias_t *alias, shift_rec_t *rec, shift_key1_t *key, do_alias_oper_t operation);
int do_shift_key1(do_alias_t *alias, shift_key1_t *key, do_alias_oper_t operation);
#ifndef USE_OLD_CHECKS
int do_shift_get2(do_alias_t *alias, shift_rec_t *rec, shift_key2_t *key, do_alias_oper_t operation);
int do_shift_key2(do_alias_t *alias, shift_key2_t *key, do_alias_oper_t operation);
int do_shift_get3(do_alias_t *alias, shift_rec_t *rec, shift_key3_t *key, do_alias_oper_t operation);
int do_shift_key3(do_alias_t *alias, shift_key3_t *key, do_alias_oper_t operation);
#endif
int do_checksum_get0(do_alias_t *alias, checksum_rec_t *rec, checksum_key0_t *key, do_alias_oper_t operation);
int do_checksum_key0(do_alias_t *alias, checksum_key0_t *key, do_alias_oper_t operation);
int do_checksum_get1(do_alias_t *alias, checksum_rec_t *rec, checksum_key1_t *key, do_alias_oper_t operation);
int do_checksum_key1(do_alias_t *alias, checksum_key1_t *key, do_alias_oper_t operation);
int do_check_get0(do_alias_t *alias, check_rec_t *rec, check_key0_t *key, do_alias_oper_t operation);
int do_check_key0(do_alias_t *alias, check_key0_t *key, do_alias_oper_t operation);
int do_check_get1(do_alias_t *alias, check_rec_t *rec, check_key1_t *key, do_alias_oper_t operation);
int do_check_key1(do_alias_t *alias, check_key1_t *key, do_alias_oper_t operation);
int do_check_get2(do_alias_t *alias, check_rec_t *rec, check_key2_t *key, do_alias_oper_t operation);
int do_check_key2(do_alias_t *alias, check_key2_t *key, do_alias_oper_t operation);
int do_realization_get0(do_alias_t *alias, realization_rec_t *rec, realization_key0_t *key, do_alias_oper_t operation);
int do_realization_key0(do_alias_t *alias, realization_key0_t *key, do_alias_oper_t operation);
int do_realization_get1(do_alias_t *alias, realization_rec_t *rec, realization_key1_t *key, do_alias_oper_t operation);
int do_realization_key1(do_alias_t *alias, realization_key1_t *key, do_alias_oper_t operation);
int do_realization_get2(do_alias_t *alias, realization_rec_t *rec, realization_key2_t *key, do_alias_oper_t operation);
int do_realization_key2(do_alias_t *alias, realization_key2_t *key, do_alias_oper_t operation);
int do_realization_get3(do_alias_t *alias, realization_rec_t *rec, realization_key3_t *key, do_alias_oper_t operation);
int do_realization_key3(do_alias_t *alias, realization_key3_t *key, do_alias_oper_t operation);
int do_realization_get4(do_alias_t *alias, realization_rec_t *rec, realization_key4_t *key, do_alias_oper_t operation);
int do_realization_key4(do_alias_t *alias, realization_key4_t *key, do_alias_oper_t operation);
int do_discount_get0(do_alias_t *alias, discount_rec_t *rec, discount_key0_t *key, do_alias_oper_t operation);
int do_discount_key0(do_alias_t *alias, discount_key0_t *key, do_alias_oper_t operation);
int do_discount_get1(do_alias_t *alias, discount_rec_t *rec, discount_key1_t *key, do_alias_oper_t operation);
int do_discount_key1(do_alias_t *alias, discount_key1_t *key, do_alias_oper_t operation);
#endif
int do_document_insert(do_alias_t *alias, document_rec_t *rec);
int do_document_update(do_alias_t *alias, document_rec_t *rec);
int do_document_delete(do_alias_t *alias);
int do_document_order_insert(do_alias_t *alias, document_order_rec_t *rec);
int do_document_order_update(do_alias_t *alias, document_order_rec_t *rec);
int do_document_order_delete(do_alias_t *alias);
int do_document_link_insert(do_alias_t *alias, document_link_rec_t *rec);
int do_document_link_update(do_alias_t *alias, document_link_rec_t *rec);
int do_document_link_delete(do_alias_t *alias);
int do_enum_insert(do_alias_t *alias, enum_rec_t *rec);
int do_enum_update(do_alias_t *alias, enum_rec_t *rec);
int do_enum_delete(do_alias_t *alias);
int do_document_view_insert(do_alias_t *alias, document_view_rec_t *rec);
int do_document_view_update(do_alias_t *alias, document_view_rec_t *rec);
int do_document_view_delete(do_alias_t *alias);
int do_document_data_insert(do_alias_t *alias, document_data_rec_t *rec);
int do_document_data_update(do_alias_t *alias, document_data_rec_t *rec);
int do_document_data_delete(do_alias_t *alias);
int do_product_insert(do_alias_t *alias, product_rec_t *rec);
int do_product_update(do_alias_t *alias, product_rec_t *rec);
int do_product_delete(do_alias_t *alias);
int do_product_link_insert(do_alias_t *alias, product_link_rec_t *rec);
int do_product_link_update(do_alias_t *alias, product_link_rec_t *rec);
int do_product_link_delete(do_alias_t *alias);
int do_product_view_insert(do_alias_t *alias, product_view_rec_t *rec);
int do_product_view_update(do_alias_t *alias, product_view_rec_t *rec);
int do_product_view_delete(do_alias_t *alias);
int do_product_data_insert(do_alias_t *alias, product_data_rec_t *rec);
int do_product_data_update(do_alias_t *alias, product_data_rec_t *rec);
int do_product_data_delete(do_alias_t *alias);
int do_barcode_insert(do_alias_t *alias, barcode_rec_t *rec);
int do_barcode_update(do_alias_t *alias, barcode_rec_t *rec);
int do_barcode_delete(do_alias_t *alias);
int do_account_insert(do_alias_t *alias, account_rec_t *rec);
int do_account_update(do_alias_t *alias, account_rec_t *rec);
int do_account_delete(do_alias_t *alias);
int do_subaccount_insert(do_alias_t *alias, subaccount_rec_t *rec);
int do_subaccount_update(do_alias_t *alias, subaccount_rec_t *rec);
int do_subaccount_delete(do_alias_t *alias);
int do_class_insert(do_alias_t *alias, class_rec_t *rec);
int do_class_update(do_alias_t *alias, class_rec_t *rec);
int do_class_delete(do_alias_t *alias);
int do_group_insert(do_alias_t *alias, group_rec_t *rec);
int do_group_update(do_alias_t *alias, group_rec_t *rec);
int do_group_delete(do_alias_t *alias);
int do_subgroup_insert(do_alias_t *alias, subgroup_rec_t *rec);
int do_subgroup_update(do_alias_t *alias, subgroup_rec_t *rec);
int do_subgroup_delete(do_alias_t *alias);
int do_region_insert(do_alias_t *alias, region_rec_t *rec);
int do_region_update(do_alias_t *alias, region_rec_t *rec);
int do_region_delete(do_alias_t *alias);
int do_otdel_insert(do_alias_t *alias, otdel_rec_t *rec);
int do_otdel_update(do_alias_t *alias, otdel_rec_t *rec);
int do_otdel_delete(do_alias_t *alias);
int do_partner_insert(do_alias_t *alias, partner_rec_t *rec);
int do_partner_update(do_alias_t *alias, partner_rec_t *rec);
int do_partner_delete(do_alias_t *alias);
#ifdef USE_DB_PARNER_VIEW
int do_partner_view_insert(do_alias_t *alias, partner_view_rec_t *rec);
int do_partner_view_update(do_alias_t *alias, partner_view_rec_t *rec);
int do_partner_view_delete(do_alias_t *alias);
#endif
int do_partner_data_insert(do_alias_t *alias, partner_data_rec_t *rec);
int do_partner_data_update(do_alias_t *alias, partner_data_rec_t *rec);
int do_partner_data_delete(do_alias_t *alias);
int do_sklad_insert(do_alias_t *alias, sklad_rec_t *rec);
int do_sklad_update(do_alias_t *alias, sklad_rec_t *rec);
int do_sklad_delete(do_alias_t *alias);
int do_document_prow_insert(do_alias_t *alias, document_prow_rec_t *rec);
int do_document_prow_update(do_alias_t *alias, document_prow_rec_t *rec);
int do_document_prow_delete(do_alias_t *alias);
int do_document_type_insert(do_alias_t *alias, document_type_rec_t *rec);
int do_document_type_update(do_alias_t *alias, document_type_rec_t *rec);
int do_document_type_delete(do_alias_t *alias);
int do_stock_insert(do_alias_t *alias, stock_rec_t *rec);
int do_stock_update(do_alias_t *alias, stock_rec_t *rec);
int do_stock_delete(do_alias_t *alias);
int do_prowod_insert(do_alias_t *alias, prowod_rec_t *rec);
int do_prowod_update(do_alias_t *alias, prowod_rec_t *rec);
int do_prowod_delete(do_alias_t *alias);
int do_prowod2_insert(do_alias_t *alias, prowod2_rec_t *rec);
int do_prowod2_update(do_alias_t *alias, prowod2_rec_t *rec);
int do_prowod2_delete(do_alias_t *alias);
int do_saldo_insert(do_alias_t *alias, saldo_rec_t *rec);
int do_saldo_update(do_alias_t *alias, saldo_rec_t *rec);
int do_saldo_delete(do_alias_t *alias);
int do_protocol_insert(do_alias_t *alias, protocol_rec_t *rec);
int do_protocol_update(do_alias_t *alias, protocol_rec_t *rec);
int do_protocol_delete(do_alias_t *alias);
int do_user_insert(do_alias_t *alias, user_rec_t *rec);
int do_user_update(do_alias_t *alias, user_rec_t *rec);
int do_user_delete(do_alias_t *alias);
#ifndef DOMINO78
int do_shift_insert(do_alias_t *alias, shift_rec_t *rec);
int do_shift_update(do_alias_t *alias, shift_rec_t *rec);
int do_shift_delete(do_alias_t *alias);
int do_checksum_insert(do_alias_t *alias, checksum_rec_t *rec);
int do_checksum_update(do_alias_t *alias, checksum_rec_t *rec);
int do_checksum_delete(do_alias_t *alias);
int do_check_insert(do_alias_t *alias, check_rec_t *rec);
int do_check_update(do_alias_t *alias, check_rec_t *rec);
int do_check_delete(do_alias_t *alias);
int do_realization_insert(do_alias_t *alias, realization_rec_t *rec);
int do_realization_update(do_alias_t *alias, realization_rec_t *rec);
int do_realization_delete(do_alias_t *alias);
//int do_discount_insert(do_alias_t *alias, discount_rec_t *rec);
//int do_discount_update(do_alias_t *alias, discount_rec_t *rec);
//int do_discount_delete(do_alias_t *alias);
int do_alias_discount_create(do_alias_t *alias);
#endif
double do_prowod2_sum(do_alias_t *alias, prowod2_rec_t *rec, const int number);
int    do_prowod2_sum_set(do_alias_t *alias, prowod2_rec_t *rec, const int number, double value);
void   do_prowod2_sum_clear(do_alias_t *alias, prowod2_rec_t *rec);
double do_product_price(do_alias_t *alias, product_rec_t *rec, const int number);
int    do_product_price_set(do_alias_t *alias, product_rec_t *rec, const int number, double value);
void   do_product_price_clear(do_alias_t *alias, product_rec_t *rec);
double do_product_coef(do_alias_t *alias, product_rec_t *rec);
int    do_product_life_time(do_alias_t *alias, product_rec_t *rec, struct tm *tm);
double do_document_order_price(do_alias_t *alias, document_order_rec_t *rec, const int number);
int    do_document_order_price_set(do_alias_t *alias, document_order_rec_t *rec, const int number, double value);
void   do_document_order_price_clear(do_alias_t *alias, document_order_rec_t *rec);
double do_document_order_quant(do_alias_t *alias, document_order_rec_t *rec, const int number);
int    do_document_order_quant_set(do_alias_t *alias, document_order_rec_t *rec, const int number, double value);
void   do_document_order_quant_clear(do_alias_t *alias, document_order_rec_t *rec);
double do_stock_quant(do_alias_t *alias, stock_rec_t *rec, const int number);
double do_stock_struct_quant(do_alias_t *alias, stock_struct_t *rec, const int number);
int    do_stock_quant_set(do_alias_t *alias, stock_rec_t *rec, const int number, double value);
int    do_stock_struct_quant_set(do_alias_t *alias, stock_struct_t *rec, const int number, double value);
void   do_stock_quant_clear(do_alias_t *alias, stock_rec_t *rec);
double do_sum(const char *params, const int number, int utf8);
double do_document_sum(do_alias_t *alias, document_rec_t *rec, const int number);
void   do_document_sum_set(do_alias_t *alias, document_rec_t *rec, const int number, double value);
void   do_document_sum_clear(do_alias_t *alias, document_rec_t *rec);
int    do_document_param_date(do_alias_t *alias, document_rec_t *rec, const char *name);
double do_get_rest(do_alias_t *alias, const char *code, const char *sklad);
double do_get_retail_price_full(do_alias_t *alias, const char *code, const char *sklad);
int    do_get_rest_and_price(do_alias_t *alias, const char *base_code, sklad_rec_t *sklad, do_list_t *list, int life_time);
char *do_get_partner_name(do_alias_t *alias, const char *g_code, int code);
char *do_get_partner_name_no_conv(do_alias_t *alias, const char *g_code, int code);
char* do_fromparam(const char* src, const char* name, int utf8);
//make by perl
char *do_document_param(do_alias_t *alias, document_rec_t *rec, const char *name);
int do_document_param_int(do_alias_t *alias, document_rec_t *rec, const char *name);
double do_document_param_float(do_alias_t *alias, document_rec_t *rec, const char *name);
void do_document_param_set(do_alias_t *alias, document_rec_t *rec, const char *name, const char *value);
void do_document_param_int_set(do_alias_t *alias, document_rec_t *rec, const char *name, int value);
void do_document_param_float_set(do_alias_t *alias, document_rec_t *rec, const char *name, double value);
void do_document_params_clear(do_alias_t *alias, document_rec_t *rec);
void do_document_key0_set(document_key0_t *document_key0, document_rec_t *rec);
char *do_document_order_param(do_alias_t *alias, document_order_rec_t *rec, const char *name);
int do_document_order_param_int(do_alias_t *alias, document_order_rec_t *rec, const char *name);
double do_document_order_param_float(do_alias_t *alias, document_order_rec_t *rec, const char *name);
void do_document_order_param_set(do_alias_t *alias, document_order_rec_t *rec, const char *name, const char *value);
void do_document_order_param_int_set(do_alias_t *alias, document_order_rec_t *rec, const char *name, int value);
void do_document_order_param_float_set(do_alias_t *alias, document_order_rec_t *rec, const char *name, double value);
void do_document_order_params_clear(do_alias_t *alias, document_order_rec_t *rec);
void do_document_order_key0_set(document_order_key0_t *document_order_key0, document_order_rec_t *rec);
void do_document_order_key0_set_from_document(document_order_key0_t *document_order_key0, document_rec_t *rec);
void do_document_order_key2_set_from_document(document_order_key2_t *document_order_key2, document_rec_t *rec);
char *do_document_data_param(do_alias_t *alias, document_data_rec_t *rec, const char *name);
int do_document_data_param_int(do_alias_t *alias, document_data_rec_t *rec, const char *name);
double do_document_data_param_float(do_alias_t *alias, document_data_rec_t *rec, const char *name);
void do_document_data_param_set(do_alias_t *alias, document_data_rec_t *rec, const char *name, const char *value);
void do_document_data_param_int_set(do_alias_t *alias, document_data_rec_t *rec, const char *name, int value);
void do_document_data_param_float_set(do_alias_t *alias, document_data_rec_t *rec, const char *name, double value);
void do_document_data_params_clear(do_alias_t *alias, document_data_rec_t *rec);
char *do_product_param(do_alias_t *alias, product_rec_t *rec, const char *name);
int do_product_param_int(do_alias_t *alias, product_rec_t *rec, const char *name);
double do_product_param_float(do_alias_t *alias, product_rec_t *rec, const char *name);
void do_product_param_set(do_alias_t *alias, product_rec_t *rec, const char *name, const char *value);
void do_product_param_int_set(do_alias_t *alias, product_rec_t *rec, const char *name, int value);
void do_product_param_float_set(do_alias_t *alias, product_rec_t *rec, const char *name, double value);
void do_product_params_clear(do_alias_t *alias, product_rec_t *rec);
char *do_product_link_param(do_alias_t *alias, product_link_rec_t *rec, const char *name);
int do_product_link_param_int(do_alias_t *alias, product_link_rec_t *rec, const char *name);
double do_product_link_param_float(do_alias_t *alias, product_link_rec_t *rec, const char *name);
void do_product_link_param_set(do_alias_t *alias, product_link_rec_t *rec, const char *name, const char *value);
void do_product_link_param_int_set(do_alias_t *alias, product_link_rec_t *rec, const char *name, int value);
void do_product_link_param_float_set(do_alias_t *alias, product_link_rec_t *rec, const char *name, double value);
void do_product_link_params_clear(do_alias_t *alias, product_link_rec_t *rec);
char *do_product_data_param(do_alias_t *alias, product_data_rec_t *rec, const char *name);
int do_product_data_param_int(do_alias_t *alias, product_data_rec_t *rec, const char *name);
double do_product_data_param_float(do_alias_t *alias, product_data_rec_t *rec, const char *name);
void do_product_data_param_set(do_alias_t *alias, product_data_rec_t *rec, const char *name, const char *value);
void do_product_data_param_int_set(do_alias_t *alias, product_data_rec_t *rec, const char *name, int value);
void do_product_data_param_float_set(do_alias_t *alias, product_data_rec_t *rec, const char *name, double value);
void do_product_data_params_clear(do_alias_t *alias, product_data_rec_t *rec);
char *do_subgroup_param(do_alias_t *alias, subgroup_rec_t *rec, const char *name);
int do_subgroup_param_int(do_alias_t *alias, subgroup_rec_t *rec, const char *name);
double do_subgroup_param_float(do_alias_t *alias, subgroup_rec_t *rec, const char *name);
void do_subgroup_param_set(do_alias_t *alias, subgroup_rec_t *rec, const char *name, const char *value);
void do_subgroup_param_int_set(do_alias_t *alias, subgroup_rec_t *rec, const char *name, int value);
void do_subgroup_param_float_set(do_alias_t *alias, subgroup_rec_t *rec, const char *name, double value);
void do_subgroup_params_clear(do_alias_t *alias, subgroup_rec_t *rec);
char *do_region_param(do_alias_t *alias, region_rec_t *rec, const char *name);
int do_region_param_int(do_alias_t *alias, region_rec_t *rec, const char *name);
double do_region_param_float(do_alias_t *alias, region_rec_t *rec, const char *name);
void do_region_param_set(do_alias_t *alias, region_rec_t *rec, const char *name, const char *value);
void do_region_param_int_set(do_alias_t *alias, region_rec_t *rec, const char *name, int value);
void do_region_param_float_set(do_alias_t *alias, region_rec_t *rec, const char *name, double value);
void do_region_params_clear(do_alias_t *alias, region_rec_t *rec);
char *do_otdel_param(do_alias_t *alias, otdel_rec_t *rec, const char *name);
int do_otdel_param_int(do_alias_t *alias, otdel_rec_t *rec, const char *name);
double do_otdel_param_float(do_alias_t *alias, otdel_rec_t *rec, const char *name);
void do_otdel_param_set(do_alias_t *alias, otdel_rec_t *rec, const char *name, const char *value);
void do_otdel_param_int_set(do_alias_t *alias, otdel_rec_t *rec, const char *name, int value);
void do_otdel_param_float_set(do_alias_t *alias, otdel_rec_t *rec, const char *name, double value);
void do_otdel_params_clear(do_alias_t *alias, otdel_rec_t *rec);
char *do_partner_param(do_alias_t *alias, partner_rec_t *rec, const char *name);
int do_partner_param_int(do_alias_t *alias, partner_rec_t *rec, const char *name);
double do_partner_param_float(do_alias_t *alias, partner_rec_t *rec, const char *name);
void do_partner_param_set(do_alias_t *alias, partner_rec_t *rec, const char *name, const char *value);
void do_partner_param_int_set(do_alias_t *alias, partner_rec_t *rec, const char *name, int value);
void do_partner_param_float_set(do_alias_t *alias, partner_rec_t *rec, const char *name, double value);
void do_partner_params_clear(do_alias_t *alias, partner_rec_t *rec);

char *do_partner_data_param(do_alias_t *alias, partner_data_rec_t *rec, const char *name);
int do_partner_data_param_int(do_alias_t *alias, partner_data_rec_t *rec, const char *name);
double do_partner_data_param_float(do_alias_t *alias, partner_data_rec_t *rec, const char *name);
void do_partner_data_param_set(do_alias_t *alias, partner_data_rec_t *rec, const char *name, const char *value);
void do_partner_data_param_int_set(do_alias_t *alias, partner_data_rec_t *rec, const char *name, int value);
void do_partner_data_param_float_set(do_alias_t *alias, partner_data_rec_t *rec, const char *name, double value);
void do_partner_data_params_clear(do_alias_t *alias, partner_data_rec_t *rec);

char *do_sklad_param(do_alias_t *alias, sklad_rec_t *rec, const char *name);
int do_sklad_param_int(do_alias_t *alias, sklad_rec_t *rec, const char *name);
double do_sklad_param_float(do_alias_t *alias, sklad_rec_t *rec, const char *name);
void do_sklad_param_set(do_alias_t *alias, sklad_rec_t *rec, const char *name, const char *value);
void do_sklad_param_int_set(do_alias_t *alias, sklad_rec_t *rec, const char *name, int value);
void do_sklad_param_float_set(do_alias_t *alias, sklad_rec_t *rec, const char *name, double value);
void do_sklad_params_clear(do_alias_t *alias, sklad_rec_t *rec);
char *do_document_prow_param(do_alias_t *alias, document_prow_rec_t *rec, const char *name);
int do_document_prow_param_int(do_alias_t *alias, document_prow_rec_t *rec, const char *name);
double do_document_prow_param_float(do_alias_t *alias, document_prow_rec_t *rec, const char *name);
void do_document_prow_param_set(do_alias_t *alias, document_prow_rec_t *rec, const char *name, const char *value);
void do_document_prow_param_int_set(do_alias_t *alias, document_prow_rec_t *rec, const char *name, int value);
void do_document_prow_param_float_set(do_alias_t *alias, document_prow_rec_t *rec, const char *name, double value);
void do_document_prow_params_clear(do_alias_t *alias, document_prow_rec_t *rec);
char *do_document_type_param(do_alias_t *alias, document_type_rec_t *rec, const char *name);
int do_document_type_param_int(do_alias_t *alias, document_type_rec_t *rec, const char *name);
double do_document_type_param_float(do_alias_t *alias, document_type_rec_t *rec, const char *name);
void do_document_type_param_set(do_alias_t *alias, document_type_rec_t *rec, const char *name, const char *value);
void do_document_type_param_int_set(do_alias_t *alias, document_type_rec_t *rec, const char *name, int value);
void do_document_type_param_float_set(do_alias_t *alias, document_type_rec_t *rec, const char *name, double value);
void do_document_type_params_clear(do_alias_t *alias, document_type_rec_t *rec);
char *do_prowod_param(do_alias_t *alias, prowod_rec_t *rec, const char *name);
int do_prowod_param_int(do_alias_t *alias, prowod_rec_t *rec, const char *name);
double do_prowod_param_float(do_alias_t *alias, prowod_rec_t *rec, const char *name);
void do_prowod_param_set(do_alias_t *alias, prowod_rec_t *rec, const char *name, const char *value);
void do_prowod_param_int_set(do_alias_t *alias, prowod_rec_t *rec, const char *name, int value);
void do_prowod_param_float_set(do_alias_t *alias, prowod_rec_t *rec, const char *name, double value);
void do_prowod_params_clear(do_alias_t *alias, prowod_rec_t *rec);
double do_prowod_sum(do_alias_t *alias, prowod_rec_t *rec, int number);
void do_prowod_sum_set(do_alias_t *alias, prowod_rec_t *rec, int number, double value);
void do_prowod_sum_clear(do_alias_t *alias, prowod_rec_t *rec);

char *do_prowod2_param(do_alias_t *alias, prowod2_rec_t *rec, const char *name);
int do_prowod2_param_int(do_alias_t *alias, prowod2_rec_t *rec, const char *name);
double do_prowod2_param_float(do_alias_t *alias, prowod2_rec_t *rec, const char *name);
void do_prowod2_param_set(do_alias_t *alias, prowod2_rec_t *rec, const char *name, const char *value);
void do_prowod2_param_int_set(do_alias_t *alias, prowod2_rec_t *rec, const char *name, int value);
void do_prowod2_param_float_set(do_alias_t *alias, prowod2_rec_t *rec, const char *name, double value);
void do_prowod2_params_clear(do_alias_t *alias, prowod2_rec_t *rec);
char *do_user_param(do_alias_t *alias, user_rec_t *rec, const char *name);
int do_user_param_int(do_alias_t *alias, user_rec_t *rec, const char *name);
double do_user_param_float(do_alias_t *alias, user_rec_t *rec, const char *name);
void do_user_param_set(do_alias_t *alias, user_rec_t *rec, const char *name, const char *value);
void do_user_param_int_set(do_alias_t *alias, user_rec_t *rec, const char *name, int value);
void do_user_param_float_set(do_alias_t *alias, user_rec_t *rec, const char *name, double value);
void do_user_params_clear(do_alias_t *alias, user_rec_t *rec);
#ifdef DOMINO_ODBC
int        do_alias_openODBC(do_alias_t *alias);
int        do_alias_closeODBC(do_alias_t *alias);
int        do_alias_execODBC(do_alias_t *alias, SQLHANDLE *statementHandle, const char *sqlText,
                      SQLPOINTER valuePtr, SQLINTEGER stringLength);
//                      SQLPOINTER valuePtr = SQL_CURSOR_FORWARD_ONLY, SQLINTEGER stringLength = SQL_IS_UINTEGER);
SQLRETURN  do_alias_odbc_error(SQLSMALLINT type, SQLHANDLE handle, char *fn, ...);
int        do_alias_freeStatementHandle(SQLHANDLE statementHandle);
int        do_alias_fetchODBC(SQLHANDLE statementHandle);
int        do_alias_dataODBC(SQLHSTMT statementHandle, SQLUSMALLINT colNumber, SQLSMALLINT targetType,
                      SQLPOINTER targetValuePtr, SQLLEN bufferLength, SQLLEN *datalen);
int        do_alias_rowCountODBC(SQLHSTMT statementHandle, SQLLEN *rowCount);
#endif
void       do_alias_cache_init(do_alias_t *alias);
int        do_alias_cache_account(do_alias_t *alias, account_rec_t *account);
int        do_alias_cache_subaccount(do_alias_t *alias, subaccount_rec_t *subaccount);

int do_param_new(const char* filename);
void do_param_free();
const char *do_param(do_param_t param);
const char *do_param_by_name(const char *name);
int  do_param_int(do_param_t param);
int  do_param_exist();
int  do_param_alias_count(do_param_t param);
const char*do_param_alias(do_param_t param, int indx);
int  do_param_alias_int(do_param_t param, int indx);
char *do_product_get_base_part(const char* code);
int   do_product_get_parcel_part(const char* code);
int   do_product_is_base_owner(const char *base, const char *parcial, size_t size);
int   do_product_is_parcel(const char *code, size_t size);
int   do_product_is_base_equal(const char *code1, const char *code2, size_t size);
int   do_unit_measure_count();
int   do_unit_measure_code(const char* name);
char* do_unit_measure_name(int code);
int   do_stock_cmp(double f1, double f2);

#ifdef EXPAT
do_config_t *do_config_new(const char *encoding, const char *name);
void do_config_free(do_config_t *config);
int   do_config_read(do_config_t *config, const char *filename, int empty);
int   do_config_write(do_config_t *config, const char *filename);
int   do_config_write_text(const char *filename, const char *text);
const char  *do_config_get_kind(do_config_t *config, const char *path);
const char  *do_config_set_kind(do_config_t *config, const char *path, const char *kind);
const char  *do_config_get_attr(do_config_t *config, const char *path, const char *attr);
int    do_config_get_attr_int(do_config_t *config, const char *path, const char *attr);
double do_config_get_attr_double(do_config_t *config, const char *path, const char *attr);
int    do_config_get_attr_bool(do_config_t *config, const char *path, const char *attr);
void   do_config_set_attr(do_config_t *config, const char *path, const char *attr, const char *value);
void   do_config_set_attr_int(do_config_t *config, const char *path, const char *attr, int value);
void   do_config_set_attr_double(do_config_t *config, const char *path, const char *attr, double value);
void   do_config_set_attr_bool(do_config_t *config, const char *path, const char *attr, int value);
const  do_string_list_t *do_config_get_properties(do_config_t *config, const char *path);
do_string_list_t *do_config_get_children(do_config_t *config, const char *path);
const char  *do_config_get_property(do_config_t *config, const char *path, const char *property);
int    do_config_get_property_int(do_config_t *config, const char *path, const char *property);
double do_config_get_property_double(do_config_t *config, const char *path, const char *property);
int    do_config_get_property_bool(do_config_t *config, const char *path, const char *property);
void   do_config_set_property(do_config_t *config, const char *path, const char *property, const char *value);
void   do_config_set_property_int(do_config_t *config, const char *path, const char *property, int value);
void   do_config_set_property_double(do_config_t *config, const char *path, const char *property, double value);
void   do_config_set_property_bool(do_config_t *config, const char *path, const char *property, int value);
void   do_config_clear_property(do_config_t *config, const char *path, const char *property);
#endif

do_doctype_t *do_doctype_new(do_alias_t *alias, const char* dtype, const char* dtype1);
do_doctype_t *do_doctype_new_from_dtype(do_alias_t *alias, document_type_rec_t *dtype);
do_doctype_t *do_doctype_new_from_document_order(do_alias_t *alias, document_order_rec_t *document_order);
do_doctype_t *do_doctype_new_from_document(do_alias_t *alias, document_rec_t *document);
void do_doctype_free(do_doctype_t *doctype);
int do_doctype_rest_effect(do_doctype_t *doctype);

int do_protocol_document_order_add(do_alias_t *alias, const char *dtype, const char* unit, const char *document, int line, do_protocol_action_t action, const char *action_str);
int do_protocol_document_order_add_rec(do_alias_t *alias, document_order_rec_t *rec, do_protocol_action_t action, const char *action_str);
int do_protocol_document_order_add_key(do_alias_t *alias, document_order_key0_t *key, do_protocol_action_t action, const char *action_str);
int do_protocol_document_add(do_alias_t *alias, const char *dtype, const char* unit, const char *document, do_protocol_action_t action);
int do_protocol_document_add_rec(do_alias_t *alias, document_rec_t *rec, do_protocol_action_t action);
int do_protocol_document_add_key(do_alias_t *alias, document_key0_t *key, do_protocol_action_t action);
int do_protocol_parse(do_alias_t *alias, protocol_rec_t *protocol, do_protocol_context_t *context);
int do_protocol_parse_st(do_alias_t *alias, protocol_struct_t *protocol_st, do_protocol_context_t *context);
int do_protocol_parse_string(do_alias_t *alias, const char *objectName, const char *action, do_protocol_context_t *context);
int do_protocol_add(do_alias_t *alias, do_protocol_object_t obj, const char *obj_content,
                                       do_protocol_action_t action, const char *action_content);
int do_protocol_partner_add(do_alias_t *alias, const char *g_code, int code, do_protocol_action_t action);
int do_protocol_partner_add_rec(do_alias_t *alias, partner_rec_t *rec, do_protocol_action_t action);
int do_protocol_partner_add_key(do_alias_t *alias, partner_key0_t *key, do_protocol_action_t action);
int do_protocol_product_add(do_alias_t *alias, const char *code, do_protocol_action_t action);
int do_protocol_product_add_rec(do_alias_t *alias, product_rec_t *rec, do_protocol_action_t action);
int do_protocol_product_add_key(do_alias_t *alias, product_key0_t *key, do_protocol_action_t action);
int do_protocol_partner_add_recd(do_alias_t *alias, partner_data_rec_t *rec, do_protocol_action_t action);
int do_protocol_product_add_keyd(do_alias_t *alias, product_data_key0_t *key, do_protocol_action_t action);
int do_protocol_product_add_recv(do_alias_t *alias, product_view_rec_t *rec, do_protocol_action_t action);
int do_protocol_product_add_keyv(do_alias_t *alias, product_view_key0_t *key, do_protocol_action_t action);

int do_account_get_balance_key_from_debit_prowod(do_alias_t *alias, prowod_struct_t *prowod_st, do_balance_key_t *key);
int do_account_get_balance_key_from_credit_prowod(do_alias_t *alias, prowod_struct_t *prowod_st, do_balance_key_t *key);
int do_account_rebuild_balance(do_alias_t *alias, const char *division, do_balance_key_t *key);
int do_balance_get_key_from_prowod(do_alias_t *alias, prowod_struct_t *prowod_st, do_balance_key_t *key, do_balance_side_t side, int *err);
int do_balance_rebuild(do_alias_t *alias, do_balance_key_t *key, do_extended_break_func break_func);
int do_balance_add(do_alias_t *alias, do_balance_key_t *key, int accept, do_extended_break_func break_func);


int do_action_delete_document(do_alias_t *alias, document_key0_t *document_key0);
int do_action_delete_document_order(do_alias_t *alias, document_order_key0_t *document_order_key0);
int do_action_insert_document(do_alias_t *alias, document_rec_t *document);
int do_action_insert_document_order(do_alias_t *alias, document_order_rec_t *document_order);
int do_action_receive_cl(do_alias_t *alias, const char *filename, do_extended_break_func break_func);

do_extended_t      *do_extended_new(do_alias_t *alias);
void                do_extended_set_break_func(do_extended_t *extended, do_extended_break_func break_func, void *user_data);
void                do_extended_set_file_db(do_extended_t *extended, file_db_t file_db);
void                do_extended_set_key(do_extended_t *extended, int key_num, void *key_ptr);
void                do_extended_set_filter(do_extended_t *extended, int reject, int count, ...);
void                do_extended_set_filter_v(do_extended_t *extended, int reject, do_list_t *sizes);
void                do_extended_set_filter_term(do_extended_t *extended,
                                                  int number,
                                                  int field_type,
                                                  int field_offset,
                                                  int comparison_code,
                                                  int connector,
                                                  void *value);
void                do_extended_set_filter_term_set_value(do_extended_t *extended,
                                                  int number,
                                                  void *value);
void                do_extended_set_reject_func(do_extended_t *extended, do_extended_reject_func reject_func, void *user_data);
void                do_extended_set_fields(do_extended_t *extended, int count, ...); // size,offset
void                do_extended_set_fields_full_record(do_extended_t *extended, size_t record_size);
do_ret_list_t *do_extended_get(do_extended_t *extended, int  max_record);
void                do_extended_free(do_extended_t *extended);
void                  do_ret_list_free(do_ret_list_t *list);
int                   do_ret_list_count(do_ret_list_t *list);
void                 *do_ret_list_item_data(do_ret_list_t *list, int indx);
int                   do_ret_list_item_len(do_ret_list_t *list, int indx);
int                   do_ret_list_item_pos(do_ret_list_t *list, int indx);
// add
do_ret_list_t        *do_ret_list_add_list_and_free(do_ret_list_t *list, do_ret_list_t *add);

do_ret_list_t        *do_ret_list_static_add(do_ret_list_t *list, void *data, size_t size);
do_ret_list_t        *do_ret_list_static_new();
void                  do_ret_list_delete(do_ret_list_t *list, int indx);
void                  do_ret_list_check(do_ret_list_t *list);

rz_info_t *do_rz_info_new_from_document_order(do_alias_t *alias, document_order_rec_t *document_order);
rz_info_t *do_rz_info_new_from_document_order_key0(do_alias_t *alias, document_order_key0_t *document_order_key0);
rz_info_t *do_rz_info_new(do_alias_t *alias, const char *dtype, const char *sklad, const char *document, int line);
do_list_t *do_rz_info_list_from_barcode(do_alias_t *alias, const char* barcode);
do_list_t *do_rz_info_list_from_code(do_alias_t *alias, const char* code);
int        do_rz_info_exist_by_barcode(do_alias_t *alias, const char* barcode);
int        do_rz_info_exist_by_code(do_alias_t *alias, const char* code);
int        do_rz_info_multi_exist_by_barcode(do_alias_t *alias, const char* barcode);
void       do_rz_info_key_to_document_order_key0(rz_info_t *rz_info, do_alias_t *alias, document_order_key0_t *document_order_key0);
void       do_rz_info_key_to_document_order(rz_info_t *rz_info, do_alias_t *alias, document_order_rec_t *document_order);
void       do_rz_info_barcode_to_document_order_key0(rz_info_t *rz_info, do_alias_t *alias, document_order_key0_t *document_order_key0);
void       do_rz_info_barcode_to_document_order(rz_info_t *rz_info, do_alias_t *alias, document_order_rec_t *document_order);
void       do_rz_info_util_set_document_order_key0_from_barcode(do_alias_t *alias, document_order_key0_t *document_order_key0, const char* barcode);
do_list_t *do_rz_info_util_mark_link(do_alias_t *alias, const char *barcode);
void       do_rz_info_free(rz_info_t *rz_info);
void       do_rz_info_clear(rz_info_t *rz_info);
void       do_rz_info_list_free(do_list_t *list);
int        do_rz_info_cmp(rz_info_t *r1, rz_info_t *r2);
double     do_rate_for_date(do_alias_t *alias, time_t time, int valuta);

int   do_util_str_to_document_order_key0(do_alias_t *alias, const char *str, document_order_key0_t *key0);
int   do_util_str_to_document_key0(do_alias_t *alias, const char *str, document_key0_t *key0);
char *do_util_document_order_key0_to_str(do_alias_t *alias, document_order_key0_t *key0);
char *do_util_document_order_to_str(do_alias_t *alias, document_order_rec_t *rec);
char *do_util_document_to_str(do_alias_t *alias, document_rec_t *rec);
char *do_util_document_key0_to_str(do_alias_t *alias, document_key0_t *key0);
int   do_util_get_empty_document_number(do_alias_t *alias, document_key0_t *key);
int   do_util_ch_params(BTI_CHAR *params, BTI_WORD *size, size_t offset);
int   do_util_str_to_partner_key0(do_alias_t *alias, const char *str, partner_key0_t *partner_key0);
char *do_util_partner_key0_to_str(do_alias_t *alias, partner_key0_t *partner_key0);
int   do_util_product_get4(do_alias_t *alias, product_rec_t *product, product_key4_t *product_key4);

do_ret_list_t *do_util_product_list_get(do_alias_t *alias, partner_key0_t *partner_key0);
int            do_util_product_list_delete_product(do_alias_t *alias, partner_key0_t *partner_key0, product_key4_t *product_key4);
int            do_util_product_list_insert_product(do_alias_t *alias, partner_key0_t *partner_key0, product_key4_t *product_key4);

int   do_deaccept(do_alias_t *alias, document_key0_t *document_key0, do_deaccept_t accept, do_extended_break_func break_func);
int   do_deaccept_order(do_alias_t *alias, document_order_rec_t *document_order, do_deaccept_t accept, do_extended_break_func break_func);

#ifdef ACCEPT_DOCUMENT
int   do_accept_order(do_alias_t *alias, document_rec_t *document, document_order_rec_t *document_order, do_accept_t accept, do_accept_param_t *param);
int   do_accept(do_alias_t *alias, document_key0_t *document_key0, do_accept_t accept, do_extended_break_func break_func);
int   do_accept_make_sums_prowod(do_alias_t *alias, document_rec_t *document, do_doctype_t *doctype, do_extended_break_func break_func);
int   do_accept_make_prowod(do_alias_t *alias, document_rec_t *document, do_doctype_t *doctype, do_extended_break_func break_func);
#endif

double do_atof(const char *str);
char*  do_ftoa(const double value, char *buf);
double do_pdecimal(unsigned char *value, size_t size, int fracplaces);
int    do_pdecimal_set(unsigned char *dst, size_t size, int fracplaces, double value);
#define do_cpy(dst,src) {if (sizeof(dst) != sizeof(src)) do_log(LOG_ERR,"logical error %d != %d",sizeof(dst),sizeof(src)); memcpy(&dst,&src, sizeof(dst));}
//#define do_cpy(dst,src) {if (sizeof(dst) != sizeof(src)) do_log(LOG_ERR,"logical error %d != %d %s %d",sizeof(dst),sizeof(src),__FILE__,__LINE__); memcpy(&dst,&src, sizeof(dst));}
#define do_cmp(dst,src) memcmp(&dst,&src, sizeof(dst))
#define do_set_empty(dst) memset(&dst, 0, sizeof(dst))
#define do_sizeof(a,b) sizeof(((a*)NULL)->b)
#define do_size_and_offset(a,b) sizeof(((a*)NULL)->b), offsetof(a,b)
int do_atodate(const char *str, struct tm *tm);
int do_atotime(const char *str, struct tm *tm);
time_t do_sqltodate(int date, struct tm *tm);
time_t do_sqltotime(int time, struct tm *tm);
int do_datetosql(const struct tm tm);
int do_timetosql(const struct tm tm);
time_t do_date_set_ymd(struct tm *tm, int year, int month, int day);
char *do_datetoa(const struct tm tm);
char *do_timetoa(const struct tm tm);
char *do_datetoa_c(const struct tm *tm);
int  do_recover_print(FILE *file, void *data, int size);
char *do_ean13(const char *code);
int64_t do_real_to_money(double value);
char* do_ftoasum(const double value);

do_r3_var_t *do_r3_expr_calc(const char *expression, do_r3_var_get_value_func value_func, void *user_data);
int          do_r3_var_value_int(do_r3_var_t *var);
double       do_r3_var_value_double(do_r3_var_t *var);
char        *do_r3_var_value_string(do_r3_var_t *var);
do_r3_var_t *do_r3_var_int(int value);
do_r3_var_t *do_r3_var_double(double value);
do_r3_var_t *do_r3_var_string(const char *value);
void         do_r3_var_free(do_r3_var_t *var);

char *do_real_in_words(do_alias_t *alias, double value, int mantissa, int abr);
char *do_date_in_words(do_alias_t *alias, struct tm *tm);

int do_report_product_meta_traffic(do_meta_alias_t *meta,
                                            const char *code,
                                            int all_parcel,
                                            const char *dtype,
                                            do_string_list_t *dtype1,
                                            const char *sklad,
                                            struct tm *date_b,
                                            struct tm *date_e,
                                            int quant_n,
                                            int price_n,
                                            double *quant,
                                            double *sum);
int do_report_product_meta_traffic_partner(do_meta_alias_t *meta,
                                            const char *code,
                                            int all_parcel,
                                            const char *dtype,
                                            do_string_list_t *dtype1,
                                            const char *sklad,
                                            struct tm *date_b,
                                            struct tm *date_e,
                                            int quant_n,
                                            int price_n,
                                            double *quant,
                                            double *sum,
                                            partner_key0_t *partner_key0);
do_report_product_traffic_detail_t *do_report_product_meta_traffic_detail(do_meta_alias_t *meta,
                                            const char *code,
                                            int all_parcel,
                                            const char *dtype,
                                            do_string_list_t *dtype1,
                                            const char *sklad,
                                            struct tm *date_b,
                                            struct tm *date_e,
                                            int quant_n,
                                            int price_n,
                                            do_report_product_traffic_detail_t *val);
void do_report_product_traffic_detail_free(do_report_product_traffic_detail_t *report);
int do_report_product_traffic_detail_document_count(do_report_product_traffic_detail_t *report);
const char *do_report_product_traffic_document_p_doc(do_report_product_traffic_detail_t *report, int indx);
struct tm *do_report_product_traffic_document_p_time(do_report_product_traffic_detail_t *report, int doc_indx);
const char *do_report_product_traffic_document_sklad(do_report_product_traffic_detail_t *report, int indx);
int do_report_product_traffic_document_line_count(do_report_product_traffic_detail_t *report, int doc_indx);
double do_report_product_traffic_document_line_quant(do_report_product_traffic_detail_t *report, int doc_indx, int line_indx);
int do_report_product_traffic_document_for_partner_count(do_report_product_traffic_detail_t *report, int par_indx);
int do_report_product_traffic_document_for_partner_document(do_report_product_traffic_detail_t *report, int par_indx, int indx);
const char *do_report_product_traffic_partner_name(do_report_product_traffic_detail_t *report, int par_indx);
int do_report_product_traffic_partner_count(do_report_product_traffic_detail_t *report);


do_sort_list_t *do_report_get_rest(do_alias_t *alias, const char *unit);
void do_rest_list_free(do_sort_list_t *list);
void do_rest_free(do_rest_t *rest);



markup_info_t *do_markup_get(do_alias_t *alias, const char *unit, const char *code, const char *partner_region, int partner_code);
void           do_markup_free(markup_info_t *markup);
double         do_markup_get_rate(markup_info_t *markup, double price);
double         do_markup_get_max_rate(markup_info_t *markup, double price);

double do_markup_get_max_grls_price(do_alias_t *alias, const char *unit, int vat_rate, double price, ...);
double do_markup_get_max_grls_price_(do_alias_t *alias, const char *unit, int vat_rate, int price_len, double *prices);
void   do_markup_get_max_grls_pct(do_alias_t *alias, const char *unit, double *pct1, double *pct2, double price, ...);
void   do_markup_get_max_grls_pct_(do_alias_t *alias, const char *unit, double *pct1, double *pct2, int price_len, double *prices);

char *do_city_get_name(do_alias_t *alias, int code);
do_string_list_t *do_city_get_units(do_alias_t *alias, int code);
int do_city_get_city_by_unit(do_alias_t *alias, const char *unit);
do_list_t *do_city_get_cities(do_alias_t *alias);

char *do_firm_get_name(do_alias_t *alias, int code);
do_string_list_t *do_firm_get_units(do_alias_t *alias, int code);
int do_firm_get_firm_by_unit(do_alias_t *alias, const char *unit);
do_list_t *do_firm_get_firms(do_alias_t *alias);

char *do_get_unit_desc(do_alias_t *alias, const char *units);
do_string_list_t  *do_get_units(do_alias_t *alias);

#ifdef USE_IM
void domino_im_init();
#endif

#ifdef __cplusplus
}
#endif

#endif // DOMINO_H_INCLUDED
