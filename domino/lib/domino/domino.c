#include <domino.h>
#include <errno.h>
#include <dirent.h>
#include "../../config/config.h"
#include "../../misc/define.h"
#include "../../misc/iconv_.h"

#define END_PARAM "END"

static int init_lib = FALSE;

static struct {
    char  *config_file;
    char  *domino_param_file;
    char  *domino_alias_path;
    char  *password_file;
    char  *key_file;
    char  *local_unit;
} domino = {NULL, NULL, NULL, NULL, NULL};
static int read_config(const char* filename);
static int use_utf_charset = 0;

typedef struct {
    char *name;
    char *password;
    int   supervisore;
    do_string_list_t *groups;
    do_sort_list_t *rights;
    domino_role_t role;
} domino_user_t;

static domino_user_t crnt_user;
static char *crnt_alias_name;

static const char *key[DO_PARAM_END + 1] = {
    "DP_PARAMETR_PRODUCT",
    "DP_SKLAD_RETAIL_KIND",
    "DP_SKLAD_RETAIL_TYPE",
    "DP_PARTNER_REGION_DISCOUNT",
    "DP_PARTNER_REGION_CASH",
    "DP_PARTNER_REGION_CASHIER",

    "DP_DOCUMENT_TYPE_DISCOUNT",
    "DP_DOCUMENT_TYPE_PRODUCT_IN",
    "DP_DOCUMENT_TYPE_PRODUCT_OUT",
    "DP_DOCUMENT_TYPE_PRICE",
    "DP_DOCUMENT_TYPE_CRNTSALE",
    "DP_DOCUMENT_TYPE_ACCOUNT",
    "DP_DOCUMENT_TYPE_KEYBOARD",
    "DP_DOCUMENT_TYPE_ORDER",
    "DP_DOCUMENT_TYPE_RZ",
    "DP_DOCUMENT_TYPE_RATE",
    "DP_DOCUMENT_TYPE_PKO",
    "DP_DOCUMENT_TYPE_RKO",

    "DP_DOCUMENT_OPERATION_PRODUCT_IN_INVOICE",
    "DP_DOCUMENT_OPERATION_ACCOUNT_INVOICE",
    "DP_DOCUMENT_OPERATION_CRNTSALE_DEFAULT",

    "DOCUMENT_PARAMETR_COMMENT",
    "DOCUMENT_PARAMETR_INVOICE",
    "DOCUMENT_PARAMETR_INVOICE_DATE",

    "DOCUMENT_PARAMETR_DISCOUNT_NAME",
    "DOCUMENT_PARAMETR_DISCOUNT_KIND",
    "DOCUMENT_PARAMETR_DISCOUNT_CALC",
    "DOCUMENT_PARAMETR_DISCOUNT_SCALE",
    "DOCUMENT_PARAMETR_DISCOUNT_PERC",
    "DOCUMENT_PARAMETR_DISCOUNT_LIMIT_PRODUCT",
    "DOCUMENT_PARAMETR_DISCOUNT_LIMIT_SUM",
    "DOCUMENT_PARAMETR_DISCOUNT_LIMIT_PERIOD",
    "DOCUMENT_PARAMETR_DISCOUNT_LIMIT_SUM_LOWER",
    "DOCUMENT_PARAMETR_DISCOUNT_LIMIT_SUM_UPPER",
    "DOCUMENT_PARAMETR_DISCOUNT_LIMIT_SKLAD",

    "DOCUMENT_PARAMETR_KEYBOARD_KIND",
    "DOCUMENT_PARAMETR_KEYBOARD_DESC",
    "DOCUMENT_PARAMETR_KEYBOARD_PRODUCT",
    "DOCUMENT_PARAMETR_KEYBOARD_COMMAND",
    "DOCUMENT_PARAMETR_KEYBOARD_COMMAND_PARAMETERS",
    "DOCUMENT_PARAMETR_KEYBOARD_FONT",

    "DOCUMENT_PARAMETR_CHECK_SUM",
    "DOCUMENT_PARAMETR_CHECK_TOTAL",
    "DOCUMENT_PARAMETR_CASHIER",
    "DOCUMENT_PARAMETR_INFO_PAY",
    "DOCUMENT_PARAMETR_SUM_PURCHASE",
    "DOCUMENT_PARAMETR_SUM_NDS_PURCHASE",

    "DORDER_PARAMETR_CASHIER",
    "DORDER_PARAMETR_OPERATION",
    "DORDER_PARAMETR_CHECK",
    "DORDER_PARAMETR_BARCODE",
    "DORDER_PARAMETR_POSITION",
    "DORDER_PARAMETR_MNN",
    "DORDER_PARAMETR_TRADE_NAME",
    "DORDER_PARAMETR_PACK_FORM",
    "DORDER_PARAMETR_VALUTA_CODE",
    "DORDER_PARAMETR_MANUFACTOR",
    "DORDER_PARAMETR_SRC_FILENAME",
    "DORDER_PARAMETR_NN",
    "DORDER_PARAMETR_UNIQ_NX",
    "DORDER_PARAMETR_UNPACK",
    "DORDER_PARAMETR_NDS",
    "DORDER_PARAMETR_NDS_SUPPLIER",
    "DORDER_PARAMETR_TOTAL_PACK",
    "DORDER_PARAMETR_SERIA",
    "DORDER_PARAMETR_LIFE_TIME",
    "DORDER_PARAMETR_FACT_RZ_PRICE",


    "PROWOD_PARAMETR_DOCUMENT",
    "PROWOD_PARAMETR_COMMENT",


    "LINE_PARAMETR_DISCOUNT_KIND",
    "LINE_PARAMETR_DISCOUNT_REDEFINE",

    "PRICE_PURCHASE",
    "PRICE_FACT",
    "PRICE_RETAIL_DEFAULT",

    "QUANT_DEFAULT",
    "QUANT_CRNTSALE",
    "QUANT_PRICE_PACK",
    "QUANT_PRICE",

    "SKLAD_PARAMETR_PRICE_RETAIL",
    "SKLAD_PARAMETR_KIND",
    "SKLAD_PARAMETR_KEYBOARD_WIDTH",
    "SKLAD_PARAMETR_KEYBOARD_HEIGHT",
    "SKLAD_PARAMETR_KEYBOARD_COL",
    "SKLAD_PARAMETR_KEYBOARD_TEMPLATE",
    "SKLAD_PARAMETR_PV_DEBIT_ACCOUNT",
    "SKLAD_PARAMETR_PV_CREDIT_ACCOUNT",
    "SKLAD_PARAMETR_PV_ACCOUNT",
    "SKLAD_PARAMETR_PV_BANK_IN",
    "SKLAD_PARAMETR_PV_BANK_IN_OSB",
    "SKLAD_PARAMETR_PV_BANK_IN_BIK",
    "SKLAD_PARAMETR_PV_BANK_OUT",
    "SKLAD_PARAMETR_PV_BANK_OUT_OSB",
    "SKLAD_PARAMETR_PV_BANK_OUT_BIK",
    "SKLAD_PARAMETR_POST",

    "PARTNER_PARAMETR_SKLAD",
    "PARTNER_PARAMETR_CASH_CONNECTION_STRING",
    "PARTNER_PARAMETR_CASH_IP",
    "PARTNER_PARAMETR_CASHIER_LOGIN",
    "PARTNER_PARAMETR_CASHIER_PASSWORD",
    "PARTNER_PARAMETR_CASHIER_ROLE",
    "PARTNER_PARAMETR_CASHIER_FIO",
    "PARTNER_PARAMETR_FIRM_DIRECTOR",
    "PARTNER_PARAMETR_FIRM_MANAGER",
    "PARTNER_PARAMETR_FIRM_BOOKKEEPER",
    "PARTNER_PARAMETR_FIRM_CASHIER",
    "PARTNER_PARAMETR_FIRM_NAME",
    "PARTNER_PARAMETR_FIRM_INN_KPP",
    "PARTNER_PARAMETR_FIRM_ADDRESS",
    "PARTNER_PARAMETR_FIRM_INN",
    "PARTNER_PARAMETR_FIRM_KPP",
    "PARTNER_PARAMETR_FIRM_OKATO",
    "PARTNER_PARAMETR_FIRM_FULL_NAME",
    "PARTNER_PARAMETR_FIRM_OKPO",
    "PARTNER_PARAMETR_FIRM_DIRECTOR_POST",
    "PARTNER_PARAMETR_FIRM_DIRECTOR_FIO",
    "PARTNER_PARAMETR_FIRM_BOOK_FIO",
    "PARTNER_PARAMETR_FIRM_S_POST",
    "PARTNER_PARAMETR_FIRM_S_FIO",


    "PROTOCOL_ACTION_INSERT",
    "PROTOCOL_ACTION_INSERT_LINE",
    "PROTOCOL_ACTION_ACCEPT",
    "PROTOCOL_ACTION_DEACCEPT",
    "PROTOCOL_ACTION_CHANGE",
    "PROTOCOL_ACTION_DELETE",
    "PROTOCOL_ACTION_DELETE_LINE",
    "PROTOCOL_ACTION_CHANGE_LINE",
    "PROTOCOL_ACTION_CHANGE_KEY",
    "PROTOCOL_ACTION_CHANGE_LOCAL",
    "PROTOCOL_ACTION_LOGIN",
    "PROTOCOL_ACTION_LOGOUT",
    "PROTOCOL_ACTION_REPLIC",

    "PROTOCOL_OBJECT_PRODUCT",
    "PROTOCOL_OBJECT_SKLAD",
    "PROTOCOL_OBJECT_REGION",
    "PROTOCOL_OBJECT_PARTNER",
    "PROTOCOL_OBJECT_CLASS",
    "PROTOCOL_OBJECT_GROUP",
    "PROTOCOL_OBJECT_SUBGROUP",
    "PROTOCOL_OBJECT_DOCUMENT",
    "PROTOCOL_OBJECT_CHECK",
    "PROTOCOL_OBJECT_SYSTEM",
    "PROTOCOL_OBJECT_REPLIC",
    "PROTOCOL_OBJECT_DOCUMENT_LINK",
    "PROTOCOL_OBJECT_PARTNER_DATA",
    "PROTOCOL_OBJECT_PRODUCT_DATA",
    "PROTOCOL_OBJECT_DOCUMENT_DATA",
    "PROTOCOL_OBJECT_DOCUMENT_ORDER",
    "PROTOCOL_OBJECT_OTHER",

    "DP_COMMON_SKLAD",
    "DP_COMMON_DATE_ACCEPT",
    "DP_COMMON_TIME_ACCEPT",

    "PRODUCT_PARAMETR_LIFE_TIME",
    "PRODUCT_PARAMETR_SERIA",
    "PRODUCT_PARAMETR_NDS",
    "PRODUCT_PARAMETR_NDS_SUPPLIER",
    "PRODUCT_PARAMETR_CLASS",
    "PRODUCT_PARAMETR_LIST",
    "PRODUCT_PARAMETR_TEMPERATURE",
    "PRODUCT_PARAMETR_IMPERATIVE",
    "PRODUCT_PARAMETR_NO_QUANT_LIMIT",

    "DP_PRODUCT_BASE_PARCEL_SEPARATOR",
    "DP_PRODUCT_BASE_CODE_LENGTH",
    //"DP_PRODUCT_PARCEL_CODE_FORMAT",

    "DP_DOCUMENT_P_CASH_FORMAT",

    "OTDEL_PARAMETR_FIRM",
    "OTDEL_PARAMETR_NUMBER_FORMAT",


    "CL_PARAMETR_DATE",
    "CL_PARAMETR_DATE_B",
    "CL_PARAMETR_TIME",
    "CL_PARAMETR_CASH",
    "CL_PARAMETR_SHIFT",
    "CL_PARAMETR_CASHIER",

    "ACCOUNT_PRODUCT_IN",
    "ACCOUNT_REST_IN",
    "ACCOUNT_SUM",
    "ACCOUNT_SFF",


    "VAR_PARAMETR",
    "VAR_SUM",
    "VAR_DOCUMENT_PARTNER",
    "VAR_SKLAD_PARTNER",
    "VAR_DOCUMENT",
    "VAR_P_DOCUMENT_DATE_INT",
    "VAR_COMMENT",
    "VAR_P_DOCUMENT",
    "VAR_SF_ANALIT",
    "VAR_DOCUMENT_DATE",
    "VAR_SF_NUMBER",
    "VAR_SF_DATE",

    "DPROWOD_PARAMETR_DEBIT_ACCOUNT",
    "DPROWOD_PARAMETR_DEBIT_SUB_ACCOUNT",
    "DPROWOD_PARAMETR_DEBIT_PARTNER",
    "DPROWOD_PARAMETR_DEBIT_ANALIT",
    "DPROWOD_PARAMETR_CREDIT_ACCOUNT",
    "DPROWOD_PARAMETR_CREDIT_SUB_ACCOUNT",
    "DPROWOD_PARAMETR_CREDIT_PARTNER",
    "DPROWOD_PARAMETR_CREDIT_ANALIT",
    "DPROWOD_PARAMETR_CREDIT_SUM_1",
    "DPROWOD_PARAMETR_CREDIT_SUM_2",
    "DPROWOD_PARAMETR_CREDIT_SUM_3",
    "DPROWOD_PARAMETR_CREDIT_VED",
    "DPROWOD_PARAMETR_CREDIT_COMMENT",
    "DPROWOD_PARAMETR_CREDIT_DOCUMENT",
    "DPROWOD_PARAMETR_CREDIT_DATE",
    "DPROWOD_PARAMETR_CREDIT_CONDITION_1",
    "DPROWOD_PARAMETR_CREDIT_CONDITION_2",

    "NAME_SCALE",
	"NAME_MAX_SCALE",
	"NAME_KIND",
	"NAME_KIND_MAX",
	"NAME_SUPPLIER",
	"NAME_RATE",
	"NAME_MAX_RATE",
	"NAME_GROUP",
	"NAME_SUBGROUP",
	"NAME_PRICE",
	"NAME_VAT",
    "NAME_ARM",
    "NAME_MAIN",
    "NAME_MAIN_SHORT",
    "NAME_OPERATOR",
    "NAME_MANAGER",
    "NAME_BOOK",


    "END"
 // ALL_DP must be last item

};


DO_EXPORT domino_role_t domino_get_role()
{
    return crnt_user.role;
}
DO_EXPORT int domino_check_role(int roles)
{
    if ( crnt_user.supervisore || crnt_user.role & roles )
        return TRUE;

    if ( (crnt_user.role & DOMINO_ROLE_MAIN_OPERATOR) &&
         (roles & DOMINO_ROLE_OPERATOR) )
         return TRUE;

    if ( (crnt_user.role & DOMINO_ROLE_MAIN_MANAGER) &&
         (roles & DOMINO_ROLE_MANAGER) )
         return TRUE;

    if ( (crnt_user.role & DOMINO_ROLE_MAIN_BOOK) &&
         (roles & DOMINO_ROLE_BOOK) )
         return TRUE;

    return FALSE;
}


static char *domino_get_config_dir(char *localname, char *filename)
{
#ifndef _WIN32
   char *localfilename = NULL;
   char *config = getenv("XDG_CONFIG_HOME");
   DIR *file;
   if ( config && config[0] )
       localfilename=do_strdup_printf("%s/%s", config, localname);
   else {
       config = getenv("HOME");
       if ( config && config[0] )
           localfilename=do_strdup_printf("%s/.config/%s", config, localname);
   }
   if ( localfilename ) {
        if ( (file = opendir(localfilename)) == NULL ) {
            do_free(localfilename);
            return DOMINO_CONFIG(filename);
        } 
        else {
            closedir(file);
            return localfilename;
        }
   }
   else 
#endif
        return  DOMINO_CONFIG(filename);
}
DO_EXPORT int domino_init(const char *filename, int use_utf, do_param_t end)
{
    use_utf_charset = use_utf;

    if (end != DO_PARAM_END) {
	do_log(LOG_ERR, "End of param not correct %d != %d", end, DO_PARAM_END);
	return 0;
    }
    if ( !key[DO_PARAM_END] ) {
	do_log(LOG_ERR, "End of param not init");
	return 0;
    }
    product_struct_t product;
    if ( sizeof(product) != 2470 ) {
	do_log(LOG_ERR, "The library is compiled without __attribute__((packed)) sizeof product struct %d", sizeof(product));
	return 0;
    }
    if (domino.domino_param_file) do_free(domino.domino_param_file);
    if (domino.domino_alias_path) do_free(domino.domino_alias_path);
    if (domino.local_unit) do_free(domino.local_unit);
    if (domino.password_file) do_free(domino.password_file);
    if (domino.key_file) do_free(domino.key_file);
    domino.key_file  = NULL;

    domino.domino_param_file = do_get_config_filename(DOMINO_PARAM_LOCAL_CONFIG_FILE,DOMINO_PARAM_CONFIG_FILE);
    domino.domino_alias_path  = domino_get_config_dir(DOMINO_ALIAS_LOCAL_PATH,DOMINO_ALIAS_RELATIVE_PATH);
    domino.password_file  = do_get_config_filename(DOMINO_PASSWD_LOCAL_FILE,DOMINO_PASSWD_FILE);

    crnt_user.name = do_strdup(DOMINO_USERNAME);
    crnt_user.password = do_strdup("");
    crnt_user.supervisore = 0;
    crnt_user.role = DOMINO_ROLE_HANDBOOK;
    crnt_user.groups = do_string_list_new();
    crnt_user.rights = do_sort_list_new(0, 1, NULL, NULL);

    crnt_alias_name = NULL;

    domino.local_unit = do_strdup("");
    if ( filename )
        domino.config_file = do_strdup(filename);
    else 
        domino.config_file = do_get_config_filename(DOMINO_CONFIG_LOCAL_FILE,DOMINO_CONFIG_FILE);
    

    if (!read_config(domino.config_file))
        return 0;
    if (!domino.domino_param_file) {
        do_log(LOG_ERR, "undefine \"domino-param-file\" - domino parameters file");
        return 0;
    }
    if (!domino.domino_alias_path) {
        do_log(LOG_ERR, "undefine \"domino-alias-path\" - directory to find alias description");
        return 0;
    }
    if (!domino.local_unit) {
        do_log(LOG_ERR, "undefine \"local-unit\" - local subunit");
        return 0;
    }
    if (!domino.password_file) {
        do_log(LOG_ERR, "undefine \"password-file\" - password file");
        return 0;
    }
    if (!domino.key_file) {
        do_log(LOG_ERR, "undefine \"key-file\" - key file");
        return 0;
    }
    if (do_param_exist())
        do_param_free();
    if (!do_param_new(domino.domino_param_file))
        return 0;
    if (strcmp(do_param(DO_PARAM_END), END_PARAM)) {
	do_log(LOG_ERR, "params not correct \"%s\"", do_param(DO_PARAM_END));
	return 0;
    }
    do_set_passwd_filename(domino.password_file);
    init_lib = TRUE;

    return 1;

}
DO_EXPORT int domino_init_light(const char* local_unit, int use_utf, do_param_t end)
{
    use_utf_charset = use_utf;

    if (end != DO_PARAM_END) {
	do_log(LOG_ERR, "End of param not correct %d != %d", end, DO_PARAM_END);
	return 0;
    }
    if ( !key[DO_PARAM_END] ) {
	do_log(LOG_ERR, "End of param not init");
	return 0;
    }
    product_struct_t product;
    if ( sizeof(product) != 2470 ) {
	do_log(LOG_ERR, "The library is compiled without __attribute__((packed)) sizeof product struct %d", sizeof(product));
	return 0;
    }
    if (domino.domino_param_file) do_free(domino.domino_param_file);
    if (domino.domino_alias_path) do_free(domino.domino_alias_path);
    if (domino.local_unit) do_free(domino.local_unit);
    if (domino.password_file) do_free(domino.password_file);
    if (domino.key_file) do_free(domino.key_file);
    domino.key_file  = NULL;

    domino.domino_alias_path  = NULL;
    domino.password_file  = NULL;
    
    domino.domino_param_file = do_get_config_filename(".", DOMINO_PARAM_CONFIG_FILE_NAME);
    if ( !do_param_new(domino.domino_param_file) )
        return 0;

    crnt_user.name = do_strdup(DOMINO_USERNAME);
    crnt_user.password = do_strdup("");
    crnt_user.supervisore = 0;
    crnt_user.role = DOMINO_ROLE_HANDBOOK;
    crnt_user.groups = do_string_list_new();
    crnt_user.rights = do_sort_list_new(0, 1, NULL, NULL);

    crnt_alias_name = NULL;

    domino.local_unit = do_strdup(local_unit);
    init_lib = TRUE;
    return 1;

}
DO_EXPORT void domino_finalize()
{
    if (domino.config_file) do_free(domino.config_file);
    if (domino.domino_param_file) do_free(domino.domino_param_file);
    if (domino.domino_alias_path) do_free(domino.domino_alias_path);
    if (domino.local_unit) do_free(domino.local_unit);
    if (domino.password_file) do_free(domino.password_file);
    if (domino.key_file) do_free(domino.key_file);
    if (do_param_exist())
        do_param_free();
    if (crnt_user.name)
        do_free(crnt_user.name);
    if (crnt_user.password)
        do_free(crnt_user.password);
    if (crnt_user.groups)
        do_string_list_free(crnt_user.groups);
    if (crnt_user.rights)
        do_sort_list_free(crnt_user.rights);
}
const char *division = "";
DO_EXPORT const char *domino_division()
{
    return division;
}
DO_EXPORT const char *domino_unit()
{
    return domino.local_unit;
}

DO_EXPORT const char *domino_key_file()
{
    return domino.key_file;
}
do_alias_t *do_alias_new(const char* dbConfDir, const char* aliasName, int breakOnError, const char *username, const char* password, int utf);
do_alias_t *do_alias_new_light(int breakOnError, const char *username, const char* password, int utf);
#ifndef NO_USE_DOMINO_DB
DO_EXPORT do_alias_t *domino_alias_new(const char* name)
{
    if ( !init_lib )
        if ( !domino_init(NULL, TRUE, DO_PARAM_END) ) {
           do_log(LOG_ERR, "Окружение домино  не инициализировано");
           return NULL;
         }

    if (!name)
        return do_alias_new(domino.domino_alias_path, crnt_alias_name, 1, crnt_user.name, crnt_user.password, use_utf_charset);
    else
        return do_alias_new(domino.domino_alias_path, name, 1, crnt_user.name, crnt_user.password, use_utf_charset);
}
DO_EXPORT do_alias_t *domino_alias_new_light(const char *local_unit)
{
    if ( !init_lib )
        if ( !domino_init_light(local_unit, TRUE, DO_PARAM_END) ) {
           do_log(LOG_ERR, "Окружение домино  не инициализировано");
           return NULL;
         }

    return do_alias_new_light(1, crnt_user.name, crnt_user.password, use_utf_charset);
}
void do_alias_set_host(do_alias_t *alias, const char *host);
int  do_alias_change_dbname(do_alias_t *alias, const char *dbname);
DO_EXPORT int domino_alias_set_store_number(do_alias_t *alias, int number, const char *hostname, const char *dbname)
{
    char *host;
    char *db;
    int ret;
    if ( hostname ) 
        host = do_strdup(hostname);
    else 
        host = do_strdup_printf("%s%d", "mag", number);
    if ( dbname ) 
        db = do_strdup(dbname);
    else 
        db = do_strdup_printf("%s%d", "maria", number);
    do_alias_set_host(alias, host);
    ret = do_alias_change_dbname(alias, db);
    do_free(host); do_free(db);
    return ret;
}

DO_EXPORT void domino_set_alias_name(const char* name)
{
    if (crnt_alias_name)
        do_free(crnt_alias_name);
    if (name)
        crnt_alias_name = do_strdup(name);
    else
        crnt_alias_name = NULL;
}
DO_EXPORT void domino_set_unit(const char* code)
{
    if ( domino.local_unit )
        do_free(domino.local_unit);
    domino.local_unit = do_strdup(code);
}

DO_EXPORT int domino_get_unit(do_alias_t *alias, sklad_rec_t *sklad, const char *code)
{
    int status;
    sklad_key0_t sklad_key0;
    do_text_set(alias, sklad_key0.code, code);

    status = do_sklad_get0(alias, sklad, &sklad_key0, DO_GET_EQUAL);
    if (status == DO_KEY_NOT_FOUND)
        do_log(LOG_ERR, "cant found sklad \"%s\"", domino.local_unit);
    return status == DO_OK;
}
DO_EXPORT char *domino_get_unit_prefix(do_alias_t *alias , const char *sklad_code)
{
    int status;
    sklad_key0_t sklad_key0;
    sklad_rec_t sklad;

    do_text_set(alias, sklad_key0.code, sklad_code);

    status = do_sklad_get0(alias, &sklad, &sklad_key0, DO_GET_EQUAL);
    if (status == DO_KEY_NOT_FOUND)
        do_log(LOG_ERR, "cant found sklad \"%s\"", domino.local_unit);

    if (status != DO_OK)
        return NULL;
    return do_text(alias, sklad.data.TOW_PREFIX);
}
#endif

#define DELSPACE while (cp[0] == ' ') ++cp
static int read_config(const char* filename)
{
    FILE *fp;
    if ((fp = fopen(filename, "r")) == NULL)  {
        do_log(LOG_ERR, "Error opening configuration file (%s): %s", filename, strerror(errno));
        return 0;
    }
    size_t len = BUFSIZ;
    char* line = (char*) do_malloc(len + 1);
    int error = 0;
    int retval = 1;
    while (!feof(fp))  {
        char* cp,*element,*value,*tail;
        if (fgets(line, len+1, fp) == NULL) break;
        while ((strlen(line) == len) && !feof(fp)) {
            line = (char *)do_realloc(line, len * 2 + 1);
            if (!fgets(line+len, len+1, fp)) {
                error = 1;
                retval = feof(fp);
                break;
            }
            len *= 2;
        }
        if (error)
            break;
        cp = line;
        DELSPACE;
        /* A leading '#' or ';' signals a comment line.  */
        if (cp[0] == '#' || cp[0] == ';' || cp[0] == '\0' || cp[0] == '\n')
            continue;
        element = cp;
        while (cp[0] != '\0' && cp[0] != '\n' && cp[0] != ' ')
           ++cp;
        tail = cp - 1;
        while (tail != element && (tail[0] == ' ' || tail[0] == '\n' || tail[0] == '\r' ))
           --tail;
        if (element == tail)
            continue;
        DELSPACE;
        value = cp;
        while (cp[0] != '\0' && cp[0] != '\n') ++cp;
        while (cp != value && (cp[0] == ' ' || cp[0] == '\n' || cp[0] == '\r'))   --cp;
        element[tail - element + 1] = '\0';
        value[cp - value + 1] = '\0';

        if (!strcmp(element,"local-unit")) {
            if (domino.local_unit) do_free(domino.local_unit);
            domino.local_unit = do_strdup(value);
        }
        else
        if (!strcmp(element,"domino-param-file")) {
            if (domino.domino_param_file) do_free(domino.domino_param_file);
            domino.domino_param_file = do_strdup(value);
        }
        else
        if (!strcmp(element,"domino-alias-path")) {
            if (domino.domino_alias_path) do_free(domino.domino_alias_path);
            domino.domino_alias_path = do_strdup(value);
        }
        else
        if (!strcmp(element,"password-file")) {
            if (domino.password_file) do_free(domino.password_file);
            domino.password_file = do_strdup(value);
        }
        else
        if (!strcmp(element,"key-file")) {
            if (domino.key_file) do_free(domino.key_file);
            domino.key_file = do_strdup(value);
        }
    }
    do_free(line);
    fclose(fp);
    return retval;
}
static int write_config()
{
    FILE *fp, *fo;
    int found= FALSE;
    if ((fp = fopen(domino.config_file, "r")) == NULL)  {
        do_log(LOG_ERR, "Error opening configuration file (%s): %s", domino.config_file, strerror(errno));
        return 0;
    }
    char *outfile;
    outfile = do_strdup_printf("%s.new", domino.config_file);
    if ((fo = fopen(outfile, "w+")) == NULL)  {
        do_log(LOG_ERR, "Error write configuration file (%s): %s", outfile, strerror(errno));
        do_free(outfile);
        return 0;
    }
    size_t len = BUFSIZ;
    char* line = (char*) do_malloc(len + 1);
    int error = 0;
    int retval = 1;
    while (!feof(fp))  {
        char* cp,*element,*value,*tail;
        if (fgets(line, len+1, fp) == NULL) break;
        while ((strlen(line) == len) && !feof(fp)) {
            line = (char *)do_realloc(line, len * 2 + 1);
            if (!fgets(line+len, len+1, fp)) {
                error = 1;
                retval = feof(fp);
                break;
            }
            len *= 2;
        }
        if ( error )
            break;
        cp = line;
        DELSPACE;
        /* A leading '#' or ';' signals a comment line.  */
        if (cp[0] == '#' || cp[0] == ';' || cp[0] == '\0' || cp[0] == '\n') {
            fprintf(fo, "%s", line);
            continue;
        }
        element = cp;
        while (cp[0] != '\0' && cp[0] != '\n' && cp[0] != ' ')
           ++cp;
        tail = cp - 1;
        while (tail != element && (tail[0] == ' ' || tail[0] == '\n' || tail[0] == '\r' ))
           --tail;
        if ( element == tail ) {
            fprintf(fo, "%s", line);
            continue;
        }
        DELSPACE;
        value = cp;
        while (cp[0] != '\0' && cp[0] != '\n') ++cp;
        while (cp != value && (cp[0] == ' ' || cp[0] == '\n' || cp[0] == '\r'))   --cp;

        if (!strncmp(element,"local-unit",tail - element + 1)) {
            fprintf(fo, "local-unit %s\n", domino.local_unit);
            found = TRUE;
        }
        else {
            fprintf(fo, "%s", line);
            continue;
        }
    }
    if ( !found )
        fprintf(fo, "local-unit %s\n", domino.local_unit);
    do_free(line);
    fclose(fp);
    fclose(fo);
    char *tmpfile;
    tmpfile = do_strdup_printf("%s~", domino.config_file);
    remove(tmpfile);
    if (rename (domino.config_file, tmpfile) == -1 ) {
        do_log(LOG_ERR, "rename file %s to %s %s", domino.config_file,tmpfile, strerror(errno));
        retval = FALSE;
    }
    if (retval && (rename (outfile, domino.config_file) == -1)) {
        do_log(LOG_ERR, "rename file %s to %s %s", outfile, domino.config_file, strerror(errno));
        retval = FALSE;
    }
    do_free(tmpfile);
    do_free(outfile);
    return retval;
}
DO_EXPORT int domino_auth(const char *name, const char *password)
{
    int retval;
    char *groups;
    char *rights;
    int super, i;
    retval = do_alias_auth(domino.domino_alias_path, NULL, name, password, &groups, &rights, &super, use_utf_charset);
    if (retval == 1) {
        do_free(crnt_user.name);
        crnt_user.name = do_strdup(name);
        do_free(crnt_user.password);
        crnt_user.password = do_strdup(password);
        crnt_user.supervisore = super;
        crnt_user.role = DOMINO_ROLE_HANDBOOK;
        char *ch = groups, *head;
        do_string_list_clear(crnt_user.groups);
        while (*ch != '\0') {
            head = ch;
            while (*ch != '\0' && *ch != ';') ch++;
            if (head != ch)
                do_string_list_addn_alloc(crnt_user.groups, head, ch - head);
            if (*ch == ';') ch++;
        }
        if (crnt_user.supervisore)
            crnt_user.role = DOMINO_ROLE_SUPERVISOR;
        else
            for (i = 0; i < crnt_user.groups->count; i++) {
                char *str;
                str = do_strdup_printf("%s.%s",do_param(DO_PARAM_NAME_ARM), do_param(DO_PARAM_NAME_OPERATOR));

                if ( !strncmp(crnt_user.groups->list[i], str, strlen(str)) && (crnt_user.role < DOMINO_ROLE_OPERATOR) )
                   crnt_user.role = DOMINO_ROLE_OPERATOR;
                do_free(str);
                str = do_strdup_printf("%s.%s",do_param(DO_PARAM_NAME_ARM), do_param(DO_PARAM_NAME_MANAGER));
                if ( !strncmp(crnt_user.groups->list[i], str, strlen(str)) && (crnt_user.role < DOMINO_ROLE_MANAGER) )
                   crnt_user.role = DOMINO_ROLE_MANAGER;
                do_free(str);
                str = do_strdup_printf("%s.%s",do_param(DO_PARAM_NAME_ARM), do_param(DO_PARAM_NAME_BOOK));
                if ( !strncmp(crnt_user.groups->list[i], str, strlen(str)) && (crnt_user.role < DOMINO_ROLE_BOOK) )
                   crnt_user.role = DOMINO_ROLE_BOOK;
                do_free(str);
                str = do_strdup_printf("%s.%s.%s",do_param(DO_PARAM_NAME_ARM), do_param(DO_PARAM_NAME_MAIN_SHORT), do_param(DO_PARAM_NAME_OPERATOR));
                if ( !strncmp(crnt_user.groups->list[i], str, strlen(str)) && (crnt_user.role < DOMINO_ROLE_MAIN_OPERATOR) )
                   crnt_user.role = DOMINO_ROLE_MAIN_OPERATOR;
                do_free(str);
                str = do_strdup_printf("%s.%s.%s",do_param(DO_PARAM_NAME_ARM), do_param(DO_PARAM_NAME_MAIN_SHORT), do_param(DO_PARAM_NAME_MANAGER));
                if ( !strncmp(crnt_user.groups->list[i], str, strlen(str)) && (crnt_user.role < DOMINO_ROLE_MAIN_MANAGER) )
                   crnt_user.role = DOMINO_ROLE_MAIN_MANAGER;
                do_free(str);
                str = do_strdup_printf("%s.%s.%s",do_param(DO_PARAM_NAME_ARM), do_param(DO_PARAM_NAME_MAIN_SHORT), do_param(DO_PARAM_NAME_BOOK));
                if ( !strncmp(crnt_user.groups->list[i], str, strlen(str)) && (crnt_user.role < DOMINO_ROLE_MAIN_BOOK) )
                   crnt_user.role = DOMINO_ROLE_MAIN_BOOK;
                do_free(str);
            }
    }
    return retval;
}
DO_EXPORT do_string_list_t *domino_user_groups()
{
    return crnt_user.groups;
}

DO_EXPORT int domino_nretail_price(do_alias_t *alias, const char* sklad_code)
{
    sklad_rec_t sklad;
    sklad_key0_t sklad_key0;
    do_text_set(alias, sklad_key0.code, sklad_code);
    int status = do_sklad_get0(alias, &sklad, &sklad_key0, DO_GET_EQUAL);
    if (status != DO_OK) {
        if (status == DO_KEY_NOT_FOUND)
            do_log(LOG_ERR, "Sklad \"%s\" not found", sklad_code);
        return do_param_int(DO_PARAM_PRICE_RETAIL_DEFAULT);
    }
    int nretail_price;
    if ((nretail_price = do_sklad_param_int(alias, &sklad, do_param(DO_PARAM_SKLAD_PARAMETR_PRICE_RETAIL))) == 0)
        nretail_price = do_param_int(DO_PARAM_PRICE_RETAIL_DEFAULT);
    return nretail_price;
}
#define ALIAS_SUFFIX "_ALIAS"
#define ALIAS_SUFFIX_LEN 6

static char *params[DO_PARAM_END+1];

static do_string_list_t *params_alias[DO_PARAM_END+1];

static const char *empty_str = "";

static int params_init = 0;

#define DO_UNIT_MEASURE_COUNT "UNIT_MEASURE_COUNT"
#define DO_UNIT_MEASURE_ "UNIT_MEASURE_"
static int unit_measure_count = 0;
static char **unit_measure;

DO_EXPORT void do_param_free()
{
    int i;
    for (i = 0; i <= DO_PARAM_END; i++)
        if (params[i])
            do_free(params[i]);
    for (i = 0; i < unit_measure_count; i++)
        if (unit_measure[i])
            do_free(unit_measure[i]);
    for (i = 0; i <= DO_PARAM_END; i++)
        if (params_alias[i])
            do_string_list_free(params_alias[i]);
    do_free (unit_measure);
    params_init = 0;
}
DO_EXPORT int do_param_exist()
{
    return params_init;
}


DO_EXPORT int do_param_new(const char* filename)
{
    int i;
    for (i = 0; i <= DO_PARAM_END; i++)
        params[i] = NULL;
    for (i = 0; i <= DO_PARAM_END; i++)
        params_alias[i] = NULL;
    FILE *fp;
    int defaultfname = 0;
    unit_measure_count = 0;
    char *fname = (char*) filename;
    if (!fname) {
        defaultfname = 1;
        fname = DOMINO_CONFIG(DOMINO_CONFIG_FILE);
    }
    if ((fp = fopen(fname, "r")) == NULL)  {
        do_log(LOG_ERR, "Error opening configuration file (%s): %s", fname, strerror(errno));
        if (defaultfname)
            do_free(fname);
        return 0;
    }
    const char *str;
    if (use_utf_charset)
        str = UTF_CHARSET;
    else
        str = get_locale_lang();
    iconv_t conv = iconv_open(str, DOMINO_CHARSET);
    if (conv == (iconv_t)-1)
        do_log(LOG_ERR, "Error iconv_open (\"%s\" to \"%s\") ", DOMINO_CHARSET, str);

    size_t len = 1024;
    char* line = (char *) do_malloc (len + 1);
    char* buf = NULL;
    int flag;
//!!    char *p_name, *p_value;
    int name_len;
    int error = 0;
    while (!feof(fp))  {
        if (buf) {
            do_free(buf);
            buf = NULL;
        }
        char* cp, *name, *value, *tail;
        if (fgets(line, len + 1, fp) == NULL)
            break;
        while ((strlen(line) == len) && !feof(fp)) {
            line = do_realloc(line, len * 2 + 1);
            if (!fgets(line + len, len + 1, fp)) {
                error = 1;
                break;
            }
            len *= 2;
        }
        if (error)
            break;
        buf = iconv_(conv, line);
        cp = buf;
        while (cp[0] == ' ') ++cp; // delspace
        /* A leading '#' or ';' signals a comment line.  */
        if (cp[0] == '#' || cp[0] == ';' || cp[0] == '\0' || cp[0] == '\n' || cp[0] == '\r')
            continue;
        name = cp;
        while (cp[0] != '\0' && cp[0] != '\n' && cp[0] != ' ') ++cp;
        if (name == cp)
            continue;

        name_len = cp - name;

        while (cp[0] == ' ') ++cp; // delspace
        value = cp;
        while (cp[0] != '\0' && cp[0] != '\n') ++cp;
        tail = cp - 1;
        while (tail >= value && (tail[0] == ' ' || tail[0] == '\r')) tail --;
        if (value > tail)
            continue;

        value[tail - value + 1] = '\0';
        //!!p_value = strndup(value, tail - value + 1);

        flag = 0;
        for (i = 0; i <= DO_PARAM_END; i++) {
            if ((strlen(key[i]) <= name_len) &&
               !strncmp(key[i], name, strlen(key[i]))) {
                if ( strlen(key[i]) == name_len ) {
                    params[i] = do_strdup(value);
                    flag = 1;
                    break;
                }
                if ( strlen(key[i]) == name_len - ALIAS_SUFFIX_LEN &&
                     !strncmp(name + name_len - ALIAS_SUFFIX_LEN, ALIAS_SUFFIX, ALIAS_SUFFIX_LEN) ) {
                    if (!params_alias[i])
                        params_alias[i] = do_string_list_new();
                    do_string_list_add_alloc(params_alias[i], value);
                }
            }
        }
        if (flag)
            continue;
        if (strlen(DO_UNIT_MEASURE_COUNT) == name_len &&
             !strncmp(DO_UNIT_MEASURE_COUNT, name, name_len)) {
            unit_measure_count = atoi(value);
            if (unit_measure_count > 0) {
                unit_measure = (char**)do_malloc(unit_measure_count * sizeof(char*));
                memset(unit_measure, 0, unit_measure_count * sizeof(char*));
            }
            flag = 1;
        }
        if (flag) continue;
        if (name_len > strlen(DO_UNIT_MEASURE_) &&
             !strncmp(DO_UNIT_MEASURE_, name, strlen(DO_UNIT_MEASURE_))) {
            i = atoi(name + strlen(DO_UNIT_MEASURE_));
            if (i > 0 && i <= unit_measure_count) {
                unit_measure[i - 1] = do_strdup(value);
                flag = 1;
            }
        }
        if (flag) continue;
    }
    do_free(line);
    if (buf)
        do_free(buf);
    fclose(fp);
    if (defaultfname)
        do_free(fname);
    params_init = 1;
    for (i = 0; i <= DO_PARAM_END; i++)
        if (!params[i]) {
            do_log(LOG_ERR, "Domino param \"%s\" undefined", key[i]);
            params_init = 0;
        }
    if (conv != (iconv_t)-1)
        iconv_close(conv);
    return params_init;
}

DO_EXPORT const char *do_param_by_name(const char *name)
{
    int i;
    for (i = 0; i <= DO_PARAM_END; i++)
        if (!strcmp(key[i], name))
            return params[i];
    return NULL;
}
DO_EXPORT const char *do_param(do_param_t param)
{
    if (param <= DO_PARAM_END)
        return params[param];
    else
        return (char*)empty_str;
}

DO_EXPORT int  do_param_int(do_param_t param)
{
    const char *p = do_param(param);
    if (!p)
        return 0;
    else
        return atoi(p);
}

DO_EXPORT char *do_product_get_base_part(const char* code)
{
    char *cp = (char*)code, sep = params[DO_PARAM_PRODUCT_BASE_PARCEL_SEPARATOR][0];
    while (*cp != '\0' && *cp != sep) cp++;
    char *result = (char*) do_malloc(cp - code + 1);
    strncpy(result, code, cp - code);
    result[cp - code] = '\0';
    return result;
}

DO_EXPORT int do_product_get_parcel_part(const char* code)
{
    char *cp = (char*)code, sep = params[DO_PARAM_PRODUCT_BASE_PARCEL_SEPARATOR][0];
    while (*cp != '\0' && *cp != sep) cp++;
    return atoi(cp + strlen(params[DO_PARAM_PRODUCT_BASE_PARCEL_SEPARATOR]));
}

DO_EXPORT int do_product_is_base_owner(const char *base, const char *parcial, size_t size)
{
    int indx;
    for (indx = 0; indx < size && base[indx] == parcial[indx]; indx++);
    return (indx < size) && (base[indx] == ' ') && (parcial[indx] == params[DO_PARAM_PRODUCT_BASE_PARCEL_SEPARATOR][0]);
}

DO_EXPORT int do_product_is_base_equal(const char *code1, const char *code2, size_t size)
{
    int indx;
    for (indx = 0; indx < size &&
                    code1[indx] == code2[indx] &&
                    code1[indx] != params[DO_PARAM_PRODUCT_BASE_PARCEL_SEPARATOR][0] &&
                    code1[indx] != ' ';
                    indx++);
    return (indx < size) && (code1[indx] == params[DO_PARAM_PRODUCT_BASE_PARCEL_SEPARATOR][0] || code1[indx] == ' ')
                         && (code2[indx] == params[DO_PARAM_PRODUCT_BASE_PARCEL_SEPARATOR][0] || code2[indx] == ' ');
}

DO_EXPORT int do_product_is_parcel(const char *code, size_t size)
{
    int indx;
    for (indx = 0; indx < size; indx++) {
        if (code[indx] == ' ') return 0;
        if (code[indx] == params[DO_PARAM_PRODUCT_BASE_PARCEL_SEPARATOR][0])
            return 1;
    }
    return 0;
}
DO_EXPORT int   do_unit_measure_count()
{
    return unit_measure_count;
}
DO_EXPORT int   do_unit_measure_code(const char* name)
{
    int i;
    for (i = 0; i < unit_measure_count; i++)
        if (unit_measure[i])
            if (!strcmp(unit_measure[i], name))
                return i + 1;
    return 0;
}
DO_EXPORT char* do_unit_measure_name(int code)
{
    if (code > 0 && code <= unit_measure_count && unit_measure[code - 1])
        return unit_measure[code - 1];
    return (char*)empty_str;
}
DO_EXPORT int  do_param_alias_count(do_param_t param)
{
    if (param > DO_PARAM_END || !params_alias[param])
        return 0;
    else
        return params_alias[param]->count;
}
DO_EXPORT const char *do_param_alias(do_param_t param, int indx)
{
    if (param > DO_PARAM_END || !params_alias[param] || indx >= params_alias[param]->count)
        return empty_str;
    else
        return params_alias[param]->list[indx];
}

DO_EXPORT int domino_check_configured()
{
    if ( !strcmp(domino.local_unit, "{empty}") )
        return FALSE;
    do_alias_t *alias;
    alias = domino_alias_new(NULL);
    if ( !alias )
        return FALSE;
    if ( !strcmp(do_alias_get_host(alias), "{empty}") ) {
        do_alias_free(alias);
        return FALSE;
    }
    do_alias_free(alias);
    alias = domino_alias_new("main");
    if ( !alias )
        return FALSE;
    if ( !strcmp(do_alias_get_host(alias), "{empty}") ) {
        do_alias_free(alias);
        return FALSE;
    }
    do_alias_free(alias);
    return TRUE;
}
int write_alias_config(do_alias_t *alias);
void do_alias_set_host(do_alias_t *alias, const char *host);

DO_EXPORT int domino_configure(const char* local_host, const char* main_host, const char* local_unit)
{
    if ( strcmp(domino.local_unit, local_unit) ) {
        do_free(domino.local_unit);
        domino.local_unit=do_strdup(local_unit);
        if ( !write_config() )
            return FALSE;
    }
    do_alias_t *alias;
    alias = domino_alias_new(NULL);
    if ( !alias )
        return FALSE;
    if ( strcmp(do_alias_get_host(alias), local_host) ) {
        do_alias_set_host(alias, local_host);
        if ( !write_alias_config(alias)) {
            do_alias_free(alias);
            return FALSE;
        }
    }
    do_alias_free(alias);
    alias = domino_alias_new("main");
    if ( !alias )
        return FALSE;
    if ( strcmp(do_alias_get_host(alias), main_host) ) {
        do_alias_set_host(alias, main_host);
        if ( !write_alias_config(alias)) {
            do_alias_free(alias);
            return FALSE;
        }
    }
    do_alias_free(alias);
    return TRUE;
}
DO_EXPORT int do_stock_cmp(double f1, double f2)
{
    return (f1 < f2 -0.0001) || (f1 > f2 +0.0001);
}
