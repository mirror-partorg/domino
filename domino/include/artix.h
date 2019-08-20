#ifndef ARTIX_H_INCLUDED
#define ARTIX_H_INCLUDED

#define ARTIX_UNPACK_PRICE_INDEX 50
#define ARTIX_CHARSET "CP1251"

#include <time.h>
#ifdef ARTIX_OLD
#include <ibase.h>
#endif
#include <iconv.h>
#include <dolib.h>

#define ARTIX_FIELD_SHOP_SIZE 30
#define ARTIX_FIELD_CASH_SIZE 30
#define ARTIX_FIELD_CASHIER_SIZE 30
#define ARTIX_FIELD_CASHIER_LOGIN_SIZE 20
#define ARTIX_FIELD_CASHIER_NAME_SIZE 32
#define ARTIX_FIELD_CASHIER_PASSWORD_SIZE 8
#define ARTIX_FIELD_BCODE_SIZE 20
#define ARTIX_FIELD_PRODUCT_NAME_SIZE 32
#define ARTIX_FIELD_PRODUCT_CODE_SIZE 100
#define ARTIX_FIELD_PRICE_NAME_SIZE 40
#define ARTIX_FIELD_MEASURE_NAME_SIZE 8
#define ARTIX_FIELD_CREDITCARD_SIZE 100
#define ARTIX_FIELD_DISCOUNTCARD_SIZE 100
#define ARTIX_FIELD_KEY_COMMAND_SIZE 60
#define ARTIX_FIELD_KEY_PARAMS_SIZE 512

#define ARTIX_CASHIER_ROLE_NONE  0
#define ARTIX_CASHIER_ROLE_CASHIER  1
#define ARTIX_CASHIER_ROLE_MANAGER  2

#define ARTIX_SEND_PRODUCT  1
#define ARTIX_SEND_CASHIER  2
#define ARTIX_SEND_KEYBOARD 4

typedef struct {
    char shop[ARTIX_FIELD_SHOP_SIZE];
    char cash[ARTIX_FIELD_CASH_SIZE];
    int shift;
    int close;
} artix_cash_t;

typedef struct {
    char shop[ARTIX_FIELD_SHOP_SIZE];
    char cash[ARTIX_FIELD_CASH_SIZE];
    int shift;
    char cashier[ARTIX_FIELD_CASHIER_SIZE];
    struct tm start_time;
    struct tm close_time;
} artix_shift_t;


typedef struct {
    char code[ARTIX_FIELD_CASHIER_SIZE];
    char login[ARTIX_FIELD_CASHIER_LOGIN_SIZE];
    char name[ARTIX_FIELD_CASHIER_NAME_SIZE];
    char passwd[ARTIX_FIELD_CASHIER_PASSWORD_SIZE];
    int role;
} artix_cashier_t;

typedef struct {
    char  bcode[ARTIX_FIELD_BCODE_SIZE];
    char  name[ARTIX_FIELD_PRODUCT_NAME_SIZE];
    int    measure;
    double coef;
    int    unpack_measure;
    double price;
    char   code[ARTIX_FIELD_PRODUCT_CODE_SIZE];
    double quant_limit;
} artix_product_t;

typedef struct {
    char  code[ARTIX_FIELD_PRODUCT_CODE_SIZE];
    char  name[ARTIX_FIELD_PRODUCT_NAME_SIZE];
    double price;
} artix_product_by_name_t;

typedef struct {
    char  bcode[ARTIX_FIELD_BCODE_SIZE];
    int    indx;
    double price;
//!!    char  name[ARTIX_FIELD_PRICE_NAME_SIZE];
} artix_price_t;

typedef struct {
    int    code;
    char  name[ARTIX_FIELD_MEASURE_NAME_SIZE];
    int    flag;
} artix_measure_t;

typedef struct {
    int code;
    char command[ARTIX_FIELD_KEY_COMMAND_SIZE];
    char params[ARTIX_FIELD_KEY_PARAMS_SIZE];
} artix_key_t;

typedef enum {
    ARTIX_ALIAS_SALE_ACTION_RETURN = 0,
    ARTIX_ALIAS_SALE_ACTION_SALE = 1
}artix_sale_action_t;

typedef enum {
    ARTIX_SALE_OPERATION_CASH_RETURN = 0,
    ARTIX_SALE_OPERATION_CASH_SALE = 1,
    ARTIX_SALE_OPERATION_CARD_RETURN = 4,
    ARTIX_SALE_OPERATION_CARD_SALE = 5,
    ARTIX_SALE_OPERATION_DEPARTMENT_RETURN = 6,
    ARTIX_SALE_OPERATION_DEPARTMENT_SALE = 7
} artix_sale_operation_t;

typedef struct {
    int check;
    int pos;
    struct tm time;
    char bcode[ARTIX_FIELD_BCODE_SIZE];
    char code[ARTIX_FIELD_PRODUCT_CODE_SIZE];
    double quant;
    double price;
    double sum;
    int cashier;
    int action;
    int operation;
    char  creditcard[ARTIX_FIELD_CREDITCARD_SIZE];
    char  discountcard[ARTIX_FIELD_DISCOUNTCARD_SIZE];
} artix_sale_t;

typedef struct {
    int check;
    int pos;
    struct tm time_storno;
    struct tm time;
    char bcode[ARTIX_FIELD_BCODE_SIZE];
    char code[ARTIX_FIELD_PRODUCT_CODE_SIZE];
    double quant;
    double price;
    double sum;
    int cashier;
} artix_storno_t;

typedef enum {
    ARTIX_OBJECT_PRODUCT,
    ARTIX_OBJECT_CASHIER,
    ARTIX_OBJECT_CASH,
    ARTIX_OBJECT_SHIFT,
    ARTIX_OBJECT_SALE,
    ARTIX_OBJECT_PRICE,
    ARTIX_OBJECT_MEASURE,
    ARTIX_OBJECT_KEY
} artix_object_t;

typedef enum {
    ARTIX_SELECT_TYPE_STRING,
    ARTIX_SELECT_TYPE_INTEGER,
    ARTIX_SELECT_TYPE_DOUBLE,
    ARTIX_SELECT_TYPE_TIME,
    ARTIX_SELECT_TYPE_UNSUPPORT
} artix_select_type_t;

typedef struct _artix_select_t artix_select_t;

typedef enum {
    ARTIX_ACTION_ADD,
    ARTIX_ACTION_REPLACE,
    ARTIX_ACTION_GET,
} artix_action_t;

#ifdef ARTIX_OLD
typedef struct _artix_alias_t artix_alias_t;
#endif
typedef struct _artix_session_t artix_session_t;
typedef struct _artix_info_t artix_info_t;

typedef enum {
  upsUndefine = 0,
  upsNone = 1,
  upsInProgress = 2,
  upsSuccess = 3,
  upsError = 4
} artix_info_upload_state_t;


typedef struct {
#ifdef ARTIX_OLD
    artix_alias_t *alias;
#endif
    artix_info_t *info;
} artix_t;

typedef enum {
    ARTIX_ALERT_DB_UNAVAILABLE = 0,
    ARTIX_ALERT_EXCHANGE_NOT_RUN,
    ARTIX_ALERT_EXCHANGE_ERROR,
    ARTIX_ALERT_CRNTSALE_ERROR,
    ARTIX_ALERT_DCLUB_NOT_RUN,
    ARTIX_ALERT_DCLUB_ERROR,
    ARTIX_ALERT_SAMBA_UNAVAILABLE,
    ARTIX_ALERT_OTHER
} artix_alert_code_t;

typedef struct {
    int is_set;
    artix_alert_code_t code;
    char *message;
} artix_alert_t;

#define ARTIX_KEY_NUMBER_MIN 1
#define ARTIX_KEY_NUMBER_MAX 52

#define ARTIX_KEY_FUNCTION_NUMBER_MIN 265
#define ARTIX_KEY_FUNCTION_NUMBER_MAX 300



#ifdef __cplusplus
extern "C"
{
#endif

artix_t *artix_new(const char* host, const char *cs, int info_port,
		   const char *username, const char *password, int use_utf);
artix_t *artix_new_default(const char* host, int use_utf);
void artix_free(artix_t *artix);


#ifdef ARTIX_OLD
artix_alias_t *artix_alias_new(const char *connection_string, const char *username, const char *password, int use_utf);
void artix_alias_free(artix_alias_t *alias);

artix_session_t *artix_session_new(artix_alias_t *alias, artix_object_t object, artix_action_t action);

int artix_session_commit(artix_session_t *session);
int artix_session_abort(artix_session_t *session);
void artix_session_free(artix_session_t *session);

int artix_multi_session_start(artix_alias_t *alias);
int artix_multi_session_commit(artix_alias_t *alias);
int artix_multi_session_abort(artix_alias_t *alias);

// procedure
int artix_alias_get_cash(artix_alias_t *alias, artix_cash_t *cash);
int artix_alias_get_shift(artix_alias_t *alias, artix_shift_t *shift);
int artix_alias_get_sale(artix_alias_t *alias, artix_shift_t *shift, int after_check, int after_pos, int *count, artix_sale_t **sales);
int artix_alias_get_storno(artix_alias_t *alias, artix_shift_t *shift, struct tm *tm, int *count, artix_storno_t **storno);
int artix_alias_get_sale_total(artix_alias_t *alias, artix_shift_t *shift, int *check_total, double *sum_total);
#ifdef ARTIX_DELETE_PRODUCT_PROCEDURE
int artix_alias_delete_product(artix_alias_t *alias, const char *bcode);
#endif


int artix_session_add_cashier(artix_session_t *session, artix_cashier_t *cashier);
int artix_session_add_product(artix_session_t *session, artix_product_t *product);
int artix_session_add_price(artix_session_t *session, artix_price_t *price);
int artix_session_add_measure(artix_session_t *session, artix_measure_t *measure);
int artix_session_add_key(artix_session_t *session, artix_key_t *unit);

int artix_check_product(artix_alias_t *alias, artix_product_t *product);

// end procedure

char *artix_text_set_(artix_alias_t *alias, char* text, const char *value, size_t len);
#define artix_text_set(alias,text,value) artix_text_set_(alias,text, value, sizeof(text))
char *artix_text_(artix_alias_t *alias, char* text, size_t len);
#define artix_text(alias,text) artix_text_(alias,text, sizeof(text))

int artix_alias_get_products(artix_alias_t *alias, artix_product_t **products, int *count, int sort_by_bcode);
do_list_t *artix_alias_get_products_by_name(artix_alias_t *alias, const char *name_filter);
#endif

artix_info_t *artix_info_new(const char* host, int port, int use_utf);
artix_info_t *artix_info_new_default(const char* host, int use_utf);
void artix_info_free(artix_info_t *info);

int artix_info_start_upload(artix_info_t *info);
int artix_info_success_upload(artix_info_t *info);
int artix_info_fail_upload(artix_info_t *info, char** messages, int count_mes);

int artix_info_get_upload_state(artix_info_t *info, artix_info_upload_state_t *state);
int artix_info_set_upload_state(artix_info_t *info, artix_info_upload_state_t state);

int artix_info_get_upload_progress(artix_info_t *info, int *percent);
int artix_info_set_upload_progress(artix_info_t *info, int percent);

int artix_info_get_upload_error_description(artix_info_t *info, char*** messages, int *count_mes);
int artix_info_set_upload_error_description(artix_info_t *info, char** messages, int count_mes);

int artix_info_get_available_modules(artix_info_t *info, char*** module_names, int *count_mod);
int artix_info_get_module_description(artix_info_t *info, const char *module_name, char*** module_desc, int *count_desc);

#ifndef OLD_USE_ARTIX
int            artix_info_set_alert(artix_info_t *info, artix_alert_code_t code, const char *message);
int            artix_info_clear_alert(artix_info_t *info, artix_alert_code_t code);
int            artix_info_reset_alert(artix_info_t *info);
artix_alert_t *artix_info_get_alert(artix_info_t *info);
void           artix_alert_free(artix_alert_t *alert);
#endif

int artix_info_clear_upload_error_description(artix_info_t *info);
int artix_info_add_upload_error_description(artix_info_t *info, const char *message);
void artix_info_list_string_free(char** messages, int count_mes);


char *artix_key_command_desc(const char *command);
do_string_list_t *artix_key_commands();


char *artix_key_code_to_string(int code);
int   artix_key_code_from_string(const char *name);
int   artix_key_code_min();
int   artix_key_code_max();

#ifdef ARTIX_OLD
artix_select_t     *artix_select_new(artix_alias_t *alias, const char *str);
void                artix_select_free(artix_select_t *select);
int                 artix_select_record_count(artix_select_t *select);
int                 artix_select_field_count(artix_select_t *select);
artix_select_type_t artix_select_field_type(artix_select_t *select, int indx);
artix_select_type_t artix_select_field_type_by_name(artix_select_t *select, const char *name);
const char         *artix_select_field_name(artix_select_t *select, int indx);
int                 artix_select_value_integer(artix_select_t *select, int record, int indx);
const char         *artix_select_value_string(artix_select_t *select, int record, int indx);
double              artix_select_value_double(artix_select_t *select, int record, int indx);
struct tm          *artix_select_value_time(artix_select_t *select, int record, int indx);
int                 artix_select_value_by_name_integer(artix_select_t *select, int record, const char *name);
const char         *artix_select_value_by_name_string(artix_select_t *select, int record, const char *name);
double              artix_select_value_by_name_double(artix_select_t *select, int record, const char *name);
struct tm          *artix_select_value_by_name_time(artix_select_t *select, int record, const char *name);
#endif

#ifdef __cplusplus
}
#endif

#endif // ARTIX_H_INCLUDED
