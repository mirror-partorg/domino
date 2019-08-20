#include "artix.h"
#include "../../misc/define.h"
#include "../../misc/iconv_.h"
#include "../../misc/socket.h"
#include <dolib.h>
#include <stdarg.h>
#include <string.h>
#include <locale.h>

#define    BUFLEN   1024

struct _artix_alias_t {
    isc_db_handle db;
    isc_tr_handle trans;
    iconv_t fartix_conv;
    iconv_t tartix_conv;
};

struct _artix_session_t {
    int id;
    artix_alias_t *alias;
    artix_object_t object;
    int start_trans;
    isc_stmt_handle stmt;
    XSQLDA *sqlda;
};

typedef struct {
    artix_select_type_t type;
    char *name;
    size_t len;
} field_t;

struct _artix_select_t {
    do_list_t *field;
    do_list_t *record;
};

static void fb_log(ISC_STATUS* status, const char* cformat, ...);
static int actual_len(const char* s, size_t size);
static void from_artix_init(artix_alias_t *alias, int use_utf);
static void to_artix_init(artix_alias_t *alias, int use_utf);


DO_EXPORT artix_alias_t *artix_alias_new(const char *connection_string, const char *username, const char *password, int use_utf)
{
    artix_alias_t *result = do_malloc(sizeof(artix_alias_t));
    result->db = (isc_db_handle)0;
    result->trans = (isc_tr_handle)0;
    ISC_STATUS_ARRAY status;

    char                dpb[BUFLEN];
    int                 i = 0, len;
    /* Format the dpb with the user name a password */
    dpb[i++] = isc_dpb_version1;
    dpb[i++] = isc_dpb_user_name;
    len = strlen (username);
    dpb[i++] = (char) len;
    strncpy(&(dpb[i]), username, len);
    i += len;
    dpb[i++] = isc_dpb_password;
    len = strlen (password);
    dpb[i++] = len;
    strncpy(&(dpb[i]), password, len);
    i += len;
/*
    dpb[i++] = isc_dpb_lc_ctype;
    len = strlen (ctype);
    dpb[i++] = len;
    strncpy(&(dpb[i]), ctype, len);
    i += len;

*/
    if (isc_attach_database(status, 0, connection_string, &result->db, i, dpb)) {
        fb_log(status, "Attach database %s", connection_string);
        do_free(result);
        return NULL;
    }
    from_artix_init(result, use_utf);
    to_artix_init(result, use_utf);

    return result;
}
DO_EXPORT void artix_alias_free(artix_alias_t *alias)
{
    ISC_STATUS_ARRAY status;
    if (alias->trans)
        if (isc_rollback_transaction(status, &alias->trans))
            fb_log(status, "Rollback transaction");

    isc_detach_database(status, &alias->db);
    if (alias->fartix_conv != (iconv_t) -1)
        iconv_close(alias->fartix_conv);
    if (alias->tartix_conv != (iconv_t) -1)
        iconv_close(alias->tartix_conv);
    do_free(alias);
}

DO_EXPORT int artix_alias_get_cash(artix_alias_t *alias, artix_cash_t *cash)
{
    ISC_STATUS_ARRAY status;

    isc_tr_handle  trans = alias->trans;
    if (!alias->trans)
        if (isc_start_transaction(status, &trans, 1, &alias->db, 0, NULL)) {
            fb_log(status, "Start transaction");
            return 0;
        }

    isc_stmt_handle  stmt = (isc_stmt_handle)0;
    if (isc_dsql_allocate_statement(status, &alias->db, &stmt)) {
        fb_log(status, "Allocate statement");
        isc_commit_transaction(status, &trans);
        return 0;
    }

    ISC_SHORT        flag[3] = {0, 0, 0};
    XSQLDA          *sqlda = (XSQLDA *) do_malloc(XSQLDA_LENGTH(3));

    sqlda->sqln = 3;
    sqlda->version = 1;
    int result = 1;

    char  *selstr = "SELECT shopcode,cashcode,shift FROM get_cash_info";

    if (isc_dsql_prepare(status, &trans, &stmt, 0, selstr, SQL_DIALECT_V6, sqlda)) {
        fb_log(status, "Prepare \'%s\'", selstr);
        result = 0;
    }

    sqlda->sqlvar[0].sqldata = cash->shop;
    sqlda->sqlvar[0].sqlind  = &flag[0];
    sqlda->sqlvar[0].sqltype = SQL_TEXT;
    sqlda->sqlvar[0].sqllen  = sizeof(cash->shop);

    sqlda->sqlvar[1].sqldata = cash->cash;
    sqlda->sqlvar[1].sqlind  = &flag[1];
    sqlda->sqlvar[1].sqltype = SQL_TEXT;
    sqlda->sqlvar[1].sqllen  = sizeof(cash->cash);

    sqlda->sqlvar[2].sqldata = (ISC_SCHAR*)&cash->shift;
    sqlda->sqlvar[2].sqlind  = &flag[2];
    sqlda->sqlvar[2].sqltype = SQL_LONG;
    sqlda->sqlvar[2].sqllen  = sizeof(cash->shift);

    if (result && isc_dsql_execute(status, &trans, &stmt, SQL_DIALECT_V6, NULL)) {
        fb_log(status, "Execute \'%s\'", selstr);
        result = 0;
    }

    ISC_STATUS fetch_stat = 0;
    memset(cash, 0, sizeof(artix_cash_t));

    if (result && ((fetch_stat = isc_dsql_fetch(status, &stmt, SQL_DIALECT_V6, sqlda)) != 0))  {
        if (fetch_stat != 100L)
            fb_log(status, "Fetch from %s", selstr);
        result = 0;
    }

    if (result && (flag[0] == -1 || flag[1] == -1)) {
        do_log(LOG_ERR, "Shopcode or Cashcode undeclared");
        result = 0;
    }

    if (isc_dsql_free_statement(status, &stmt, DSQL_close)) {
        fb_log(status, "Free statement");
        result = 0;
    }

    if (!alias->trans)
        if (isc_commit_transaction(status, &trans)) {
            fb_log(status, "Commit transaction");
            result = 0;
        }

    do_free(sqlda);
    return result;
}
#ifdef ARTIX_DELETE_PRODUCT_PROCEDURE
DO_EXPORT int artix_alias_delete_product(artix_alias_t *alias, const char *bcode)
{
    ISC_STATUS_ARRAY    status;

    int result = 1;

    isc_tr_handle  trans = alias->trans;
    if (!alias->trans)
        if (isc_start_transaction(status, &trans, 1, &alias->db, 0, NULL)) {
            fb_log(status, "Start transaction");
            return 0;
        }

    isc_stmt_handle  stmt = (isc_stmt_handle) NULL;
    if (isc_dsql_allocate_statement(status, &alias->db, &stmt)) {
        fb_log(status, "Allocate statement");
        isc_commit_transaction(status, &trans);
        return 0;
    }

    char exec_str[BUFLEN];

    sprintf(exec_str, "EXECUTE PROCEDURE DELETE_TMC_ITEM(\'%s\')", bcode);

    if (isc_dsql_execute_immediate(status, &alias->db, &alias->trans, 0, exec_str, SQL_DIALECT_V6, NULL))    {
        fb_log(status, "run \'%s\'", exec_str);
        result = 0;
    }

    if (isc_dsql_free_statement(status, &stmt, DSQL_close)) {
        fb_log(status, "Free statement");
        result = 0;
    }

    if (!alias->trans)
        if (isc_commit_transaction(status, &trans)) {
            fb_log(status, "Commit transaction");
            result = 0;
        }

    return result;
}
#endif

DO_EXPORT int artix_alias_get_shift(artix_alias_t *alias, artix_shift_t *shift)
{
    ISC_STATUS_ARRAY status;

    isc_tr_handle  trans = alias->trans;
    if (!alias->trans)
        if (isc_start_transaction(status, &trans, 1, &alias->db, 0, NULL)) {
            fb_log(status, "Start transaction");
            return 0;
        }

    isc_stmt_handle  stmt = (isc_stmt_handle)0;
    if (isc_dsql_allocate_statement(status, &alias->db, &stmt)) {
        fb_log(status, "Allocate statement");
        isc_commit_transaction(status, &trans);
        return 0;
    }

    ISC_SHORT        flag[3] = {0, 0, 0};
    XSQLDA          *sqlda = (XSQLDA *) do_malloc(XSQLDA_LENGTH(3));

    sqlda->sqln = 3;
    sqlda->version = 1;
    int result = 1;

    char  selstr[BUFLEN];
    char *shop, *cash;
    shop = (char*)do_malloc(sizeof(shift->shop) + 1);
    strncpy(shop, shift->shop, sizeof(shift->shop));
    shop[sizeof(shift->shop)] = '\0';
    cash = (char*)do_malloc(sizeof(shift->cash) + 1);
    strncpy(cash, shift->cash, sizeof(shift->cash));
    cash[sizeof(shift->cash)] = '\0';

    sprintf(selstr, "SELECT scode,time_beg,time_end FROM get_shift_info(\'%s\',\'%s\',%d)",
             shop, cash, shift->shift);
    do_free(shop);
    do_free(cash);
    if (isc_dsql_prepare(status, &trans, &stmt, 0, selstr, SQL_DIALECT_V6, sqlda)) {
        fb_log(status, "Prepare \'%s\'", selstr);
        result = 0;
    }
    memset(&shift->start_time, 0, sizeof(shift->start_time));
    memset(&shift->close_time, 0, sizeof(shift->close_time));

    ISC_TIMESTAMP start_time, close_time;

    sqlda->sqlvar[0].sqldata = shift->cashier;
    sqlda->sqlvar[0].sqlind  = &flag[0];
    sqlda->sqlvar[0].sqltype = SQL_TEXT;
    sqlda->sqlvar[0].sqllen  = sizeof(shift->cashier);

    sqlda->sqlvar[1].sqldata = (ISC_SCHAR*)&start_time;
    sqlda->sqlvar[1].sqlind  = &flag[1];
    sqlda->sqlvar[1].sqltype = SQL_TIMESTAMP;
    sqlda->sqlvar[1].sqllen  = sizeof(start_time);

    sqlda->sqlvar[2].sqldata = (ISC_SCHAR*)&close_time;
    sqlda->sqlvar[2].sqlind  = &flag[2];
    sqlda->sqlvar[2].sqltype = SQL_TIMESTAMP;
    sqlda->sqlvar[2].sqllen  = sizeof(close_time);

    if (result && isc_dsql_execute(status, &trans, &stmt, SQL_DIALECT_V6, NULL)) {
        fb_log(status, "Execute \'%s\'", selstr);
        result = 0;
    }

    ISC_STATUS fetch_stat = 0;
    if (result && ((fetch_stat = isc_dsql_fetch(status, &stmt, SQL_DIALECT_V6, sqlda)) != 0))  {
        if (fetch_stat != 100L)
            fb_log(status, "Fetch from %s", selstr);
        result = 0;
    }
    if (result) {
        isc_decode_timestamp(&start_time, &shift->start_time);
        isc_decode_timestamp(&close_time, &shift->close_time);
    }

    if (isc_dsql_free_statement(status, &stmt, DSQL_close)) {
        fb_log(status, "Free statement");
        result = 0;
    }

    if (!alias->trans)
        if (isc_commit_transaction(status, &trans)) {
            fb_log(status, "Commit transaction");
            result = 0;
        }

    do_free(sqlda);
    return result;
}

DO_EXPORT int artix_alias_get_sale(artix_alias_t *alias, artix_shift_t *shift, int after_check, int after_pos, int *count, artix_sale_t **sales)
{
    ISC_STATUS_ARRAY status;

    isc_tr_handle  trans = alias->trans;
    if (!alias->trans)
        if (isc_start_transaction(status, &trans, 1, &alias->db, 0, NULL)) {
            fb_log(status, "Start transaction");
            return 0;
        }

    isc_stmt_handle  stmt = (isc_stmt_handle)0;
    if (isc_dsql_allocate_statement(status, &alias->db, &stmt)) {
        fb_log(status, "Allocate statement");
        isc_commit_transaction(status, &trans);
        return 0;
    }
    ISC_SHORT flag[12+1] = {0,0,0,0,0,0,0,0,0,0,0,0,0};
    XSQLDA *sqlda = (XSQLDA *) do_malloc(XSQLDA_LENGTH(12+1));
    int result = 1;
    char  selstr[BUFLEN];
    ISC_STATUS fetch_stat = 0;
    char *shop, *cash;
    shop = (char*)do_malloc(sizeof(shift->shop) + 1);
    strncpy(shop, shift->shop, sizeof(shift->shop));
    shop[sizeof(shift->shop)] = '\0';
    cash = (char*)do_malloc(sizeof(shift->cash) + 1);
    strncpy(cash, shift->cash, sizeof(shift->cash));
    cash[sizeof(shift->cash)] = '\0';


    if (!(*count)) {

        sqlda->sqln = 1;
        sqlda->version = 1;
        if (after_check > 0)
            sprintf(selstr, "SELECT count(POSNUMBER) FROM get_shift_sales(\'%s\',\'%s\',%d) WHERE checknumber > %d or (checknumber = %d and posnumber > %d)",
                 shop, cash, shift->shift, after_check, after_check, after_pos);
        else
            sprintf(selstr, "SELECT count(POSNUMBER) FROM get_shift_sales(\'%s\',\'%s\',%d)",
                 shop, cash, shift->shift);

        if (isc_dsql_prepare(status, &trans, &stmt, 0, selstr, SQL_DIALECT_V6, sqlda)) {
            fb_log(status, "Prepare \'%s\'", selstr);
            result = 0;
        }

        sqlda->sqlvar[0].sqldata = (ISC_SCHAR*)count;
        sqlda->sqlvar[0].sqlind  = &flag[0];
        sqlda->sqlvar[0].sqltype = SQL_LONG;

        if (result && isc_dsql_execute(status, &trans, &stmt, SQL_DIALECT_V6, NULL)) {
            fb_log(status, "Execute \'%s\'", selstr);
            result = 0;
        }
        if (result && ((fetch_stat = isc_dsql_fetch(status, &stmt, SQL_DIALECT_V6, sqlda)) != 0))  {
            if (fetch_stat != 100L)
                fb_log(status, "Fetch from %s", selstr);
            result = 0;
        }
        if (result && (*count))
           *sales = (artix_sale_t*)do_malloc(sizeof(artix_sale_t)*(*count));
        flag[0] = 0;
    }

    if (isc_dsql_free_statement(status, &stmt, DSQL_close)) {
        fb_log(status, "Free statement");
        result = 0;
    }
    stmt = (isc_stmt_handle)0;
    if (isc_dsql_allocate_statement(status, &alias->db, &stmt)) {
        fb_log(status, "Allocate statement");
        isc_commit_transaction(status, &trans);
        return 0;
    }
    fetch_stat = 0;
    int indx;
    sqlda->sqln = 12 + 1;
    sqlda->version = 1;
    if (after_check > 0)
        sprintf(selstr, "SELECT CHECKNUMBER,POSNUMBER,DATETIME,BCODE,CODE,QUANTITY,PRICE,"
                           "SUMM,SCODE,DIRECTION,OPCODE,CREDITCARD,DISCCARD FROM get_shift_sales(\'%s\',\'%s\',%d) "
                           "WHERE checknumber > %d or (checknumber = %d and posnumber > %d)",
             shop, cash, shift->shift, after_check, after_check, after_pos);
    else
        sprintf(selstr, "SELECT CHECKNUMBER,POSNUMBER,DATETIME,BCODE,CODE,QUANTITY,PRICE,"
                           "SUMM,SCODE,DIRECTION,OPCODE,CREDITCARD,DISCCARD FROM get_shift_sales(\'%s\',\'%s\',%d)",
             shop, cash, shift->shift);
    do_free(shop);
    do_free(cash);

    if (isc_dsql_prepare(status, &trans, &stmt, 0, selstr, SQL_DIALECT_V6, sqlda)) {
        fb_log(status, "Prepare \'%s\'", selstr);
        result = 0;
    }

    if (isc_dsql_execute(status, &trans, &stmt, SQL_DIALECT_V6, NULL)) {
        fb_log(status, "Execute \'%s\'", selstr);
        result = 0;
    }
    ISC_TIMESTAMP time;

    sqlda->sqlvar[0].sqlind  = &flag[0];
    sqlda->sqlvar[0].sqltype = SQL_LONG;
    sqlda->sqlvar[0].sqllen  = sizeof((*sales)[0].check);

    sqlda->sqlvar[1].sqlind  = &flag[1];
    sqlda->sqlvar[1].sqltype = SQL_LONG;
    sqlda->sqlvar[1].sqllen  = sizeof((*sales)[0].pos);

    sqlda->sqlvar[2].sqllen  = sizeof(time);
    sqlda->sqlvar[2].sqlind  = &flag[2];
    sqlda->sqlvar[2].sqltype = SQL_TIMESTAMP;

    sqlda->sqlvar[3].sqllen  = sizeof((*sales)[0].bcode);
    sqlda->sqlvar[3].sqlind  = &flag[3];
    sqlda->sqlvar[3].sqltype = SQL_TEXT;

    sqlda->sqlvar[4].sqllen  = sizeof((*sales)[0].code);
    sqlda->sqlvar[4].sqlind  = &flag[4];
    sqlda->sqlvar[4].sqltype = SQL_TEXT;

    sqlda->sqlvar[5].sqllen  = sizeof((*sales)[0].quant);
    sqlda->sqlvar[5].sqlind  = &flag[5];
    sqlda->sqlvar[5].sqltype = SQL_DOUBLE;

    sqlda->sqlvar[6].sqllen  = sizeof((*sales)[0].price);
    sqlda->sqlvar[6].sqlind  = &flag[6];
    sqlda->sqlvar[6].sqltype = SQL_DOUBLE;

    sqlda->sqlvar[7].sqllen  = sizeof((*sales)[0].sum);
    sqlda->sqlvar[7].sqlind  = &flag[7];
    sqlda->sqlvar[7].sqltype = SQL_DOUBLE;

    sqlda->sqlvar[8].sqllen  = sizeof((*sales)[0].cashier);
    sqlda->sqlvar[8].sqlind  = &flag[8];
    sqlda->sqlvar[8].sqltype = SQL_LONG;

    sqlda->sqlvar[9].sqllen  = sizeof((*sales)[0].action);
    sqlda->sqlvar[9].sqlind  = &flag[9];
    sqlda->sqlvar[9].sqltype = SQL_LONG;

    sqlda->sqlvar[10].sqllen  = sizeof((*sales)[0].operation);
    sqlda->sqlvar[10].sqlind  = &flag[10];
    sqlda->sqlvar[10].sqltype = SQL_LONG;

    sqlda->sqlvar[11].sqllen  = sizeof((*sales)[0].creditcard);
    sqlda->sqlvar[11].sqlind  = &flag[11];
    sqlda->sqlvar[11].sqltype = SQL_TEXT;

    sqlda->sqlvar[12].sqllen  = sizeof((*sales)[0].discountcard);
    sqlda->sqlvar[12].sqlind  = &flag[12];
    sqlda->sqlvar[12].sqltype = SQL_TEXT;

    for (indx = 0; indx < *count && result && !fetch_stat; indx++) {

        memset(&flag, 0, sizeof(flag));

        sqlda->sqlvar[0].sqldata = (ISC_SCHAR*)&(*sales)[indx].check;
        sqlda->sqlvar[1].sqldata = (ISC_SCHAR*)&(*sales)[indx].pos;
        sqlda->sqlvar[2].sqldata = (ISC_SCHAR*)&time;
        sqlda->sqlvar[3].sqldata = (*sales)[indx].bcode;
        sqlda->sqlvar[4].sqldata = (*sales)[indx].code;
        sqlda->sqlvar[5].sqldata = (ISC_SCHAR*)&(*sales)[indx].quant;
        sqlda->sqlvar[6].sqldata = (ISC_SCHAR*)&(*sales)[indx].price;
        sqlda->sqlvar[7].sqldata = (ISC_SCHAR*)&(*sales)[indx].sum;
        sqlda->sqlvar[8].sqldata = (ISC_SCHAR*)&(*sales)[indx].cashier;
        sqlda->sqlvar[9].sqldata = (ISC_SCHAR*)&(*sales)[indx].action;
        sqlda->sqlvar[10].sqldata = (ISC_SCHAR*)&(*sales)[indx].operation;
        sqlda->sqlvar[11].sqldata = (*sales)[indx].creditcard;
        sqlda->sqlvar[12].sqldata = (*sales)[indx].discountcard;

        if ((fetch_stat = isc_dsql_fetch(status, &stmt, SQL_DIALECT_V6, sqlda)) != 0)  {
            if (fetch_stat != 100L)
                fb_log(status, "Fetch from %s", selstr);
            result = 0;
            break;
        }
        if (result)
            isc_decode_timestamp(&time, &(*sales)[indx].time);
    }
    if (isc_dsql_free_statement(status, &stmt, DSQL_close)) {
        fb_log(status, "Free statement");
        result = 0;
    }

    if (!alias->trans)
        if (isc_commit_transaction(status, &trans)) {
            fb_log(status, "Commit transaction");
            result = 0;
        }

    do_free(sqlda);
    return result;
}
DO_EXPORT int artix_alias_get_storno(artix_alias_t *alias, artix_shift_t *shift, struct tm *tm, int *count, artix_storno_t **stornos)
{
    ISC_STATUS_ARRAY status;

    isc_tr_handle  trans = alias->trans;
    if (!alias->trans)
        if (isc_start_transaction(status, &trans, 1, &alias->db, 0, NULL)) {
            fb_log(status, "Start transaction");
            return 0;
        }

    isc_stmt_handle  stmt = (isc_stmt_handle)0;
    if (isc_dsql_allocate_statement(status, &alias->db, &stmt)) {
        fb_log(status, "Allocate statement");
        isc_commit_transaction(status, &trans);
        return 0;
    }
    ISC_SHORT flag[9+1] = {0,0,0,0,0,0,0,0,0};
    XSQLDA *sqlda = (XSQLDA *) do_malloc(XSQLDA_LENGTH(9+1));
    int result = 1;
    char  selstr[BUFLEN];
    ISC_STATUS fetch_stat = 0;
    char *shop, *cash;
    shop = (char*)do_malloc(sizeof(shift->shop) + 1);
    strncpy(shop, shift->shop, sizeof(shift->shop));
    shop[sizeof(shift->shop)] = '\0';
    cash = (char*)do_malloc(sizeof(shift->cash) + 1);
    strncpy(cash, shift->cash, sizeof(shift->cash));
    cash[sizeof(shift->cash)] = '\0';


    if ( !(*count) ) {

        sqlda->sqln = 1;
        sqlda->version = 1;
        if ( tm )
            sprintf(selstr, "SELECT count(POSNUM) FROM tsdata WHERE cashcode='%s' AND shift=%d AND timestorno > '%d-%d-%d %d:%d:%d'",
                 cash, shift->shift, tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
        else
            sprintf(selstr, "SELECT count(POSNUM) FROM tsdata WHERE cashcode='%s' AND shift=%d",
                 cash, shift->shift);

        if (isc_dsql_prepare(status, &trans, &stmt, 0, selstr, SQL_DIALECT_V6, sqlda)) {
            fb_log(status, "Prepare \'%s\'", selstr);
            result = 0;
        }

        sqlda->sqlvar[0].sqldata = (ISC_SCHAR*)count;
        sqlda->sqlvar[0].sqlind  = &flag[0];
        sqlda->sqlvar[0].sqltype = SQL_LONG;

        if (result && isc_dsql_execute(status, &trans, &stmt, SQL_DIALECT_V6, NULL)) {
            fb_log(status, "Execute \'%s\'", selstr);
            result = 0;
        }
        if (result && ((fetch_stat = isc_dsql_fetch(status, &stmt, SQL_DIALECT_V6, sqlda)) != 0))  {
            if (fetch_stat != 100L)
                fb_log(status, "Fetch from %s", selstr);
            result = 0;
        }
        if (result && (*count))
           *stornos = (artix_storno_t*)do_malloc(sizeof(artix_storno_t)*(*count));
        flag[0] = 0;
    }

    if (isc_dsql_free_statement(status, &stmt, DSQL_close)) {
        fb_log(status, "Free statement");
        result = 0;
    }
    stmt = (isc_stmt_handle)0;
    if (isc_dsql_allocate_statement(status, &alias->db, &stmt)) {
        fb_log(status, "Allocate statement");
        isc_commit_transaction(status, &trans);
        return 0;
    }
    fetch_stat = 0;
    int indx;
    sqlda->sqln = 12 + 1;
    sqlda->version = 1;
/*
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
*/

    if ( tm )
        sprintf(selstr, "SELECT CHECKNUM,POSNUM,TIMESTORNO,TSTIME,BCODE,CODE,"
                        "CQUANT,PRICE, SUMB, CAST(SCODE AS INTEGER) "
                        "FROM TSDATA WHERE cashcode='%s' AND shift=%d AND timestorno > '%d-%d-%d %d:%d:%d' "
                        "ORDER BY TIMESTORNO ",
                 cash, shift->shift, tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
    else
        sprintf(selstr, "SELECT CHECKNUM,POSNUM,TIMESTORNO,TSTIME,BCODE,CODE,CQUANT,PRICE,SUMB,CAST(SCODE AS INTEGER)"
                        " FROM TSDATA WHERE cashcode='%s' AND shift=%d "
                        "ORDER BY TIMESTORNO",
                 cash, shift->shift);
    do_free(shop);
    do_free(cash);

    if (isc_dsql_prepare(status, &trans, &stmt, 0, selstr, SQL_DIALECT_V6, sqlda)) {
        fb_log(status, "Prepare \'%s\'", selstr);
        result = 0;
    }

    if (isc_dsql_execute(status, &trans, &stmt, SQL_DIALECT_V6, NULL)) {
        fb_log(status, "Execute \'%s\'", selstr);
        result = 0;
    }
    ISC_TIMESTAMP time, timestorno;

    sqlda->sqlvar[0].sqlind  = &flag[0];
    sqlda->sqlvar[0].sqltype = SQL_LONG;
    sqlda->sqlvar[0].sqllen  = sizeof((*stornos)[0].check);

    sqlda->sqlvar[1].sqlind  = &flag[1];
    sqlda->sqlvar[1].sqltype = SQL_LONG;
    sqlda->sqlvar[1].sqllen  = sizeof((*stornos)[0].pos);

    sqlda->sqlvar[2].sqllen  = sizeof(time);
    sqlda->sqlvar[2].sqlind  = &flag[2];
    sqlda->sqlvar[2].sqltype = SQL_TIMESTAMP;

    sqlda->sqlvar[3].sqllen  = sizeof(time);
    sqlda->sqlvar[3].sqlind  = &flag[3];
    sqlda->sqlvar[3].sqltype = SQL_TIMESTAMP;

    sqlda->sqlvar[4].sqllen  = sizeof((*stornos)[0].bcode);
    sqlda->sqlvar[4].sqlind  = &flag[4];
    sqlda->sqlvar[4].sqltype = SQL_TEXT;

    sqlda->sqlvar[5].sqllen  = sizeof((*stornos)[0].code);
    sqlda->sqlvar[5].sqlind  = &flag[5];
    sqlda->sqlvar[5].sqltype = SQL_TEXT;

    sqlda->sqlvar[6].sqllen  = sizeof((*stornos)[0].quant);
    sqlda->sqlvar[6].sqlind  = &flag[6];
    sqlda->sqlvar[6].sqltype = SQL_DOUBLE;

    sqlda->sqlvar[7].sqllen  = sizeof((*stornos)[0].price);
    sqlda->sqlvar[7].sqlind  = &flag[7];
    sqlda->sqlvar[7].sqltype = SQL_DOUBLE;

    sqlda->sqlvar[8].sqllen  = sizeof((*stornos)[0].sum);
    sqlda->sqlvar[8].sqlind  = &flag[8];
    sqlda->sqlvar[8].sqltype = SQL_DOUBLE;

    sqlda->sqlvar[9].sqllen  = sizeof((*stornos)[0].cashier);
    sqlda->sqlvar[9].sqlind  = &flag[9];
    sqlda->sqlvar[9].sqltype = SQL_LONG;

    for (indx = 0; indx < *count && result && !fetch_stat; indx++) {

        memset(&flag, 0, sizeof(flag));

        sqlda->sqlvar[0].sqldata = (ISC_SCHAR*)&(*stornos)[indx].check;
        sqlda->sqlvar[1].sqldata = (ISC_SCHAR*)&(*stornos)[indx].pos;
        sqlda->sqlvar[2].sqldata = (ISC_SCHAR*)&timestorno;
        sqlda->sqlvar[3].sqldata = (ISC_SCHAR*)&time;
        sqlda->sqlvar[4].sqldata = (*stornos)[indx].bcode;
        sqlda->sqlvar[5].sqldata = (*stornos)[indx].code;
        sqlda->sqlvar[6].sqldata = (ISC_SCHAR*)&(*stornos)[indx].quant;
        sqlda->sqlvar[7].sqldata = (ISC_SCHAR*)&(*stornos)[indx].price;
        sqlda->sqlvar[8].sqldata = (ISC_SCHAR*)&(*stornos)[indx].sum;
        sqlda->sqlvar[9].sqldata = (ISC_SCHAR*)&(*stornos)[indx].cashier;

        if ((fetch_stat = isc_dsql_fetch(status, &stmt, SQL_DIALECT_V6, sqlda)) != 0)  {
            if (fetch_stat != 100L)
                fb_log(status, "Fetch from %s", selstr);
            result = 0;
            break;
        }
        if (result) {
            isc_decode_timestamp(&timestorno, &(*stornos)[indx].time_storno);
            isc_decode_timestamp(&time, &(*stornos)[indx].time);
        }
    }
    if (isc_dsql_free_statement(status, &stmt, DSQL_close)) {
        fb_log(status, "Free statement");
        result = 0;
    }

    if (!alias->trans)
        if (isc_commit_transaction(status, &trans)) {
            fb_log(status, "Commit transaction");
            result = 0;
        }

    do_free(sqlda);
    return result;
}
DO_EXPORT int artix_alias_get_sale_total(artix_alias_t *alias, artix_shift_t *shift, int *check_total, double *sum_total)
{
    ISC_STATUS_ARRAY status;

    isc_tr_handle  trans = alias->trans;
    if (!alias->trans)
        if (isc_start_transaction(status, &trans, 1, &alias->db, 0, NULL)) {
            fb_log(status, "Start transaction");
            return 0;
        }

    isc_stmt_handle  stmt = (isc_stmt_handle)0;
    if (isc_dsql_allocate_statement(status, &alias->db, &stmt)) {
        fb_log(status, "Allocate statement");
        isc_commit_transaction(status, &trans);
        return 0;
    }
    ISC_SHORT flag[2] = {0,0};
    XSQLDA *sqlda = (XSQLDA *) do_malloc(XSQLDA_LENGTH(2));
    int result = 1;
    char  selstr[BUFLEN];
    ISC_STATUS fetch_stat = 0;
    char *shop, *cash;
    shop = (char*)do_malloc(sizeof(shift->shop) + 1);
    strncpy(shop, shift->shop, sizeof(shift->shop));
    shop[sizeof(shift->shop)] = '\0';
    cash = (char*)do_malloc(sizeof(shift->cash) + 1);
    strncpy(cash, shift->cash, sizeof(shift->cash));
    cash[sizeof(shift->cash)] = '\0';


    fetch_stat = 0;
    sqlda->sqln = 2;
    sqlda->version = 1;
    sprintf(selstr, "SELECT COUNT(DISTINCT CHECKNUMBER),SUM(SUMM) FROM get_shift_sales(\'%s\',\'%s\',%d)",
             shop, cash, shift->shift);
    do_free(shop);
    do_free(cash);

    if (isc_dsql_prepare(status, &trans, &stmt, 0, selstr, SQL_DIALECT_V6, sqlda)) {
        fb_log(status, "Prepare \'%s\'", selstr);
        result = 0;
    }

    if (isc_dsql_execute(status, &trans, &stmt, SQL_DIALECT_V6, NULL)) {
        fb_log(status, "Execute \'%s\'", selstr);
        result = 0;
    }

    sqlda->sqlvar[0].sqlind  = &flag[0];
    sqlda->sqlvar[0].sqltype = SQL_LONG;
    sqlda->sqlvar[0].sqllen  = sizeof(int);
    sqlda->sqlvar[0].sqldata = (ISC_SCHAR*)check_total;

    sqlda->sqlvar[1].sqlind  = &flag[1];
    sqlda->sqlvar[1].sqltype = SQL_DOUBLE;
    sqlda->sqlvar[1].sqllen  = sizeof(double);
    sqlda->sqlvar[1].sqldata = (ISC_SCHAR*)sum_total;

    if ((fetch_stat = isc_dsql_fetch(status, &stmt, SQL_DIALECT_V6, sqlda)) != 0)  {
        if (fetch_stat != 100L)
            fb_log(status, "Fetch from %s", selstr);
        result = 0;
    }

    if (isc_dsql_free_statement(status, &stmt, DSQL_close)) {
        fb_log(status, "Free statement");
        result = 0;
    }
    if (!alias->trans)
        if (isc_commit_transaction(status, &trans)) {
            fb_log(status, "Commit transaction");
            result = 0;
        }

    do_free(sqlda);
    return result;
}


DO_EXPORT artix_session_t *artix_session_new(artix_alias_t *alias, artix_object_t object, artix_action_t action)
{
    artix_session_t *session = (artix_session_t *)do_malloc(sizeof(artix_session_t));
    session->alias = alias;
    session->object = object;
    session->start_trans = 0;
    session->stmt = (isc_stmt_handle)0;
    session->sqlda = NULL;

    ISC_STATUS_ARRAY status;
    ISC_SHORT flag[1] = {0};
    char exec_str[BUFLEN];
    ISC_STATUS fetch_stat;
    int datatype, incremental, field_count = 0;
    int result = 1;

    switch (object) {
        case ARTIX_OBJECT_CASHIER:
            datatype = 1;
            break;
        case ARTIX_OBJECT_PRODUCT:
            datatype = 2;
            break;
        case ARTIX_OBJECT_MEASURE:
            datatype = 4;
            break;
        case ARTIX_OBJECT_KEY:
            datatype = 5;
            break;
        case ARTIX_OBJECT_PRICE:
            datatype = 6;
            break;
//!!
        default:
            datatype = 0;
    }
    switch (action) {
        case ARTIX_ACTION_ADD:
            incremental = 1;
            break;
        case ARTIX_ACTION_REPLACE:
            incremental = 0;
            break;
        default:
            incremental = -1;

    }
    if (datatype == 0 || incremental == -1) {
        do_log(LOG_ERR, "Begin session. Input parameter mismatch.");
        result = 0;
    }

    if (result && (!alias->trans)) {
        if (isc_start_transaction(status, &alias->trans, 1, &alias->db, 0, NULL)) {
            fb_log(status, "Start transaction");
            result = 0;
        }
        else
            session->start_trans = 1;
    }

    if (result && isc_dsql_allocate_statement(status, &alias->db, &session->stmt)) {
        fb_log(status, "Allocate statement");
        result = 0;
    }

    if (result) {
        session->sqlda = (XSQLDA *) do_malloc(XSQLDA_LENGTH(1));
        session->sqlda->sqln = 1;
        session->sqlda->version = 1;
        sprintf(exec_str, "SELECT sessionid FROM begin_session(%d,%d)", datatype, incremental);
    }

    if (result && isc_dsql_prepare(status, &alias->trans, &session->stmt, 0, exec_str, SQL_DIALECT_V6, session->sqlda)) {
        fb_log(status, "Prepare \'%s\'", exec_str);
        result = 0;
    }

    if (result) {
        session->sqlda->sqlvar[0].sqldata = (ISC_SCHAR*)&session->id;
        session->sqlda->sqlvar[0].sqlind  = &flag[0];
        session->sqlda->sqlvar[0].sqltype = SQL_LONG;
        session->sqlda->sqlvar[0].sqllen  = sizeof(session->id);

        if (isc_dsql_execute(status, &alias->trans, &session->stmt, SQL_DIALECT_V6, NULL)) {
            fb_log(status, "Execute \'%s\'", exec_str);
            result = 0;
        }
    }

    if (result && ((fetch_stat = isc_dsql_fetch(status, &session->stmt, SQL_DIALECT_V6, session->sqlda)) != 0))  {
        if (fetch_stat != 100L)
            fb_log(status, "Fetch from %s", exec_str);
        result = 0;
    }
    if (session->stmt && isc_dsql_free_statement(status, &session->stmt, DSQL_close)) {
        fb_log(status, "Free statement");
        result = 0;
    }
    else
        session->stmt = (isc_stmt_handle)0;

    if (session->sqlda) {
        do_free(session->sqlda);
        session->sqlda = NULL;
    }
    if (result) {
        session->stmt = (isc_stmt_handle)0;
        if (isc_dsql_allocate_statement(status, &alias->db, &session->stmt)) {
            fb_log(status, "Allocate statement");
            result = 0;
        }
    }
    if (result) {
        switch (object) {
            case ARTIX_OBJECT_CASHIER:
                field_count = 5;
                sprintf(exec_str, "EXECUTE PROCEDURE ADD_MOL_ITEM(?,?,0,?,?,0,?, 0,\'None\',%d)",
                        session->id);
                break;
            case ARTIX_OBJECT_PRODUCT:
                field_count = 8;
//!!                sprintf(exec_str, "EXECUTE PROCEDURE ADD_TMC_ITEM(?,0,0,0,0,0,1,?,\'\',?,?,1,?,0,1,1,999999,0,0,31,3,0,0,1,0,0,?,0,?,%d)",
                sprintf(exec_str, "EXECUTE PROCEDURE ADD_TMC_ITEM(?,1,2,3,4,5,0,?,\'\',?,?,0,?,0,0,1,?,0,0,23,3,0,0,0,0,0,?,0,?,%d)",
                        session->id);
                break;
            case ARTIX_OBJECT_MEASURE:
                field_count = 3;
                sprintf(exec_str, "EXECUTE PROCEDURE ADD_UNIT_ITEM(?,?,?,%d)",
                        session->id);
                break;
            case ARTIX_OBJECT_KEY:
                field_count = 3;
                sprintf(exec_str, "EXECUTE PROCEDURE ADD_KEY_ITEM(?,?,?,%d)",
                        session->id);
                break;
            case ARTIX_OBJECT_PRICE:
                field_count = 3;
                sprintf(exec_str, "EXECUTE PROCEDURE ADD_TPRICES_ITEM(?,?,?,\'\',%d)",
                        session->id);
                break;
            default:
                result = 0;
        }
    }
    if (result) {
        session->sqlda = (XSQLDA *) do_malloc(XSQLDA_LENGTH(field_count));
        memset(session->sqlda, 0, XSQLDA_LENGTH(field_count));
        session->sqlda->version = 1;
        session->sqlda->sqln = field_count;
    }

    if (result && isc_dsql_prepare(status, &alias->trans, &session->stmt, 0, exec_str, SQL_DIALECT_V6, session->sqlda)) {
        fb_log(status, "Prepare \'%s\'", exec_str);
        result = 0;
    }
    if (result && isc_dsql_describe_bind(status, &session->stmt, 1, session->sqlda)) {
        fb_log(status, "Describe bind \'%s\'", exec_str);
        result = 0;
    }
    if (result) {
        switch (object) {
            case ARTIX_OBJECT_CASHIER:
                session->sqlda->sqlvar[0].sqltype = SQL_TEXT;
                session->sqlda->sqlvar[0].sqllen  = ARTIX_FIELD_CASHIER_SIZE;
                session->sqlda->sqlvar[1].sqltype = SQL_TEXT;
                session->sqlda->sqlvar[1].sqllen  = ARTIX_FIELD_CASHIER_LOGIN_SIZE;
                session->sqlda->sqlvar[2].sqltype = SQL_TEXT;
                session->sqlda->sqlvar[2].sqllen  = ARTIX_FIELD_CASHIER_NAME_SIZE;
                session->sqlda->sqlvar[3].sqltype = SQL_TEXT;
                session->sqlda->sqlvar[3].sqllen  = ARTIX_FIELD_CASHIER_PASSWORD_SIZE;
                session->sqlda->sqlvar[4].sqltype = SQL_LONG;
                session->sqlda->sqlvar[4].sqllen  = sizeof(int);
                break;
            case ARTIX_OBJECT_PRODUCT:
                session->sqlda->sqlvar[0].sqltype = SQL_TEXT;
                session->sqlda->sqlvar[0].sqllen  = ARTIX_FIELD_BCODE_SIZE;
                session->sqlda->sqlvar[1].sqltype = SQL_TEXT;
                session->sqlda->sqlvar[1].sqllen  = ARTIX_FIELD_PRODUCT_NAME_SIZE;
                session->sqlda->sqlvar[2].sqltype = SQL_DOUBLE;
                session->sqlda->sqlvar[2].sqllen  = sizeof(double);
                session->sqlda->sqlvar[3].sqltype = SQL_LONG;
                session->sqlda->sqlvar[3].sqllen  = sizeof(int);
                session->sqlda->sqlvar[4].sqltype = SQL_DOUBLE;
                session->sqlda->sqlvar[4].sqllen  = sizeof(double);
                session->sqlda->sqlvar[5].sqltype = SQL_DOUBLE;
                session->sqlda->sqlvar[5].sqllen  = sizeof(double);
                session->sqlda->sqlvar[6].sqltype = SQL_LONG;
                session->sqlda->sqlvar[6].sqllen  = sizeof(int);
                session->sqlda->sqlvar[7].sqltype = SQL_TEXT;
                session->sqlda->sqlvar[7].sqllen  = ARTIX_FIELD_PRODUCT_CODE_SIZE;
                break;
            case ARTIX_OBJECT_MEASURE:
                session->sqlda->sqlvar[0].sqltype = SQL_LONG;
                session->sqlda->sqlvar[0].sqllen  = sizeof(int);
                session->sqlda->sqlvar[1].sqltype = SQL_TEXT;
                session->sqlda->sqlvar[1].sqllen  = ARTIX_FIELD_MEASURE_NAME_SIZE;
                session->sqlda->sqlvar[2].sqltype = SQL_LONG;
                session->sqlda->sqlvar[2].sqllen  = sizeof(int);
                break;
            case ARTIX_OBJECT_KEY:
                session->sqlda->sqlvar[0].sqltype = SQL_LONG;
                session->sqlda->sqlvar[0].sqllen  = sizeof(int);
                session->sqlda->sqlvar[1].sqltype = SQL_TEXT;
                session->sqlda->sqlvar[1].sqllen  = ARTIX_FIELD_KEY_COMMAND_SIZE;
                session->sqlda->sqlvar[2].sqltype = SQL_TEXT;
                session->sqlda->sqlvar[2].sqllen  = ARTIX_FIELD_KEY_PARAMS_SIZE;
                break;
            case ARTIX_OBJECT_PRICE:
                session->sqlda->sqlvar[0].sqltype = SQL_TEXT;
                session->sqlda->sqlvar[0].sqllen  = ARTIX_FIELD_BCODE_SIZE;
                session->sqlda->sqlvar[1].sqltype = SQL_LONG;
                session->sqlda->sqlvar[1].sqllen  = sizeof(int);
                session->sqlda->sqlvar[2].sqltype = SQL_DOUBLE;
                session->sqlda->sqlvar[2].sqllen  = sizeof(double);
                break;
            default:
                result = 0;
        }
    }
    if (!result) {
        artix_session_free(session);
        return NULL;
    }
    return session;
}
DO_EXPORT int artix_session_commit(artix_session_t *session)
{

    ISC_STATUS_ARRAY    status;

    int result = 1;

    if (session->stmt) {
        isc_dsql_free_statement(status, &session->stmt, DSQL_close);
        session->stmt = (isc_stmt_handle)0;
    }
    if (session->sqlda) {
        do_free(session->sqlda);
        session->sqlda = NULL;
    }

    char            exec_str[BUFLEN];
    sprintf(exec_str, "EXECUTE PROCEDURE COMMIT_SESSION(%d)", session->id);

    if (isc_dsql_execute_immediate(status, &session->alias->db, &session->alias->trans, 0, exec_str, SQL_DIALECT_V6, NULL))    {
        fb_log(status, "run \'%s\'", exec_str);
        result = 0;
    }

    if (session->start_trans) {
        if (isc_commit_transaction(status, &session->alias->trans))  {
            fb_log(status, "Commit transaction");
            result = 0;
        }
        session->alias->trans = (isc_tr_handle)0;
        session->start_trans = 0;
    }
    return result;
}
DO_EXPORT int artix_session_abort(artix_session_t *session)
{
    ISC_STATUS_ARRAY    status;

    if (session->stmt) {
        isc_dsql_free_statement(status, &session->stmt, DSQL_close);
        session->stmt = (isc_stmt_handle)0;
    }
    if (session->sqlda) {
        do_free(session->sqlda);
        session->sqlda = NULL;
    }

    if (!session->start_trans)
        return 1;

    session->start_trans = 0;

    if (isc_rollback_transaction(status, &session->alias->trans))    {
        fb_log(status, "Rollback transaction");
        session->alias->trans = (isc_tr_handle)0;
        return 0;
    }
    session->alias->trans = (isc_tr_handle)0;
    return 1;
}

DO_EXPORT void artix_session_free(artix_session_t *session)
{
    ISC_STATUS_ARRAY    status;
    if (session->stmt)
        isc_dsql_free_statement(status, &session->stmt, DSQL_close);
    if (session->sqlda)
        do_free(session->sqlda);
    if (session->start_trans) {
        if (isc_rollback_transaction(status, &session->alias->trans))
            fb_log(status, "Rollback transaction");
        session->alias->trans = (isc_tr_handle)0;
    }
    do_free(session);
}

DO_EXPORT int artix_multi_session_start(artix_alias_t *alias)
{
    ISC_STATUS_ARRAY    status;
    if (alias->trans) {
        do_log(LOG_ERR, "Another transaction is active");
        return 0;
    }
    if (isc_start_transaction(status, &alias->trans, 1, &alias->db, 0, NULL)) {
        fb_log(status, "Start transaction");
        return 0;
    }
    return 1;
}
DO_EXPORT int artix_multi_session_commit(artix_alias_t *alias)
{
    ISC_STATUS_ARRAY    status;
    if (!alias->trans) {
        do_log(LOG_ERR, "Transaction is not active");
        return 0;
    }

    if (isc_commit_transaction(status, &alias->trans))    {
        fb_log(status, "Commit transaction");
        alias->trans = (isc_tr_handle)0;
        return 0;
    }
    alias->trans = (isc_tr_handle)0;
    return 1;
}
DO_EXPORT int artix_multi_session_abort(artix_alias_t *alias)
{
    if (!alias->trans) {
        do_log(LOG_ERR, "Transaction is not active");
        return 0;
    }

    ISC_STATUS_ARRAY    status;
    if (isc_rollback_transaction(status, &alias->trans))    {
        fb_log(status, "Rollback transaction");
        alias->trans = (isc_tr_handle)0;
        return 0;
    }
    alias->trans = (isc_tr_handle)0;
    return 1;
}


DO_EXPORT int artix_session_add_cashier(artix_session_t *session, artix_cashier_t *cashier)
{
    ISC_STATUS_ARRAY status;
    ISC_SHORT flags[5] = {0, 0, 0, 0, 0};

    session->sqlda->sqlvar[0].sqldata = cashier->code;
    session->sqlda->sqlvar[0].sqlind = &flags[0];
    session->sqlda->sqlvar[1].sqldata = cashier->login;
    session->sqlda->sqlvar[1].sqlind = &flags[1];
    session->sqlda->sqlvar[2].sqldata = cashier->name;
    session->sqlda->sqlvar[2].sqlind = &flags[2];
    session->sqlda->sqlvar[3].sqldata = cashier->passwd;
    session->sqlda->sqlvar[3].sqlind = &flags[3];
    session->sqlda->sqlvar[4].sqldata = (ISC_SCHAR*)&cashier->role;
    session->sqlda->sqlvar[4].sqlind = &flags[4];

    if (isc_dsql_execute(status, &session->alias->trans, &session->stmt, SQL_DIALECT_V6, session->sqlda)) {
        fb_log(status, "Execute add cashier");
        return 0;
    }
    return 1;
}

DO_EXPORT int artix_session_add_price(artix_session_t *session, artix_price_t *price)
{
    ISC_STATUS_ARRAY status;
    ISC_SHORT flags[3] = {0, 0, 0};

    session->sqlda->sqlvar[0].sqldata = price->bcode;
    session->sqlda->sqlvar[0].sqlind = &flags[0];
    session->sqlda->sqlvar[1].sqldata = (ISC_SCHAR*)&price->indx;
    session->sqlda->sqlvar[1].sqlind = &flags[1];
    session->sqlda->sqlvar[2].sqldata = (ISC_SCHAR*)&price->price;
    session->sqlda->sqlvar[2].sqlind = &flags[2];

    if (isc_dsql_execute(status, &session->alias->trans, &session->stmt, SQL_DIALECT_V6, session->sqlda)) {
        fb_log(status, "Execute add price");
        return 0;
    }
    return 1;
}

DO_EXPORT int artix_session_add_measure(artix_session_t *session, artix_measure_t *measure)
{
    ISC_STATUS_ARRAY status;
    ISC_SHORT flags[3] = {0, 0, 0};

    session->sqlda->sqlvar[0].sqldata = (ISC_SCHAR*)&measure->code;
    session->sqlda->sqlvar[0].sqlind = &flags[0];
    session->sqlda->sqlvar[1].sqldata = measure->name;
    session->sqlda->sqlvar[1].sqlind = &flags[1];
    session->sqlda->sqlvar[2].sqldata = (ISC_SCHAR*)&measure->flag;
    session->sqlda->sqlvar[2].sqlind = &flags[2];

    if (isc_dsql_execute(status, &session->alias->trans, &session->stmt, SQL_DIALECT_V6, session->sqlda)) {
        fb_log(status, "Execute add product");
        return 0;
    }
    return 1;
}
static int actual_len(const char* s, size_t size)
{
    char *ch = (char*)s + size - 1;
    while (ch > s && *ch == ' ') ch--;
    return ch - s + 1;
}

DO_EXPORT int artix_session_add_key(artix_session_t *session, artix_key_t *key)
{
    ISC_STATUS_ARRAY status;
    ISC_SHORT flags[3] = {0, 0, 0};

    session->sqlda->sqlvar[0].sqldata = (ISC_SCHAR*)&key->code;
    session->sqlda->sqlvar[0].sqlind = &flags[0];
    session->sqlda->sqlvar[1].sqldata = key->command;
    session->sqlda->sqlvar[1].sqlind = &flags[1];
    session->sqlda->sqlvar[1].sqllen = actual_len(key->command, sizeof(key->command));
    session->sqlda->sqlvar[2].sqldata = key->params;
    session->sqlda->sqlvar[2].sqlind = &flags[2];
    session->sqlda->sqlvar[2].sqllen = actual_len(key->params, sizeof(key->params));

    if (isc_dsql_execute(status, &session->alias->trans, &session->stmt, SQL_DIALECT_V6, session->sqlda)) {
        fb_log(status, "Execute add key");
        return 0;
    }
    return 1;
}


DO_EXPORT char *artix_text_set_(artix_alias_t *alias, char* text, const char *src, size_t len)
{
    char *buf = iconv_(alias->tartix_conv, src);
    if (strlen(buf) >= len)
        memcpy(text, buf, len);
    else
    {
        memset( text, 32, len);
        memcpy(text, buf, strlen(buf));
    }
    do_free(buf);
    return text;
}
DO_EXPORT char *artix_text_(artix_alias_t *alias, char* text, size_t size)
{
    char* buf = (char*)do_malloc(size + 1);
    buf[size] = '\0';
    strncpy(buf, (char*)text, size);
    char* cp = buf + size - 1;
    while ((cp > buf) && (*cp == ' ')) --cp;
    *(cp+1) = '\0';
    char *res = iconv_(alias->fartix_conv, buf);
    do_free(buf);
    return res;
}

DO_EXPORT int  artix_session_add_product(artix_session_t *session, artix_product_t *product)
{
    ISC_STATUS_ARRAY status;
    ISC_SHORT flags[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    session->sqlda->sqlvar[0].sqldata = product->bcode;
    session->sqlda->sqlvar[0].sqlind = &flags[0];

    session->sqlda->sqlvar[1].sqldata = product->name;
    session->sqlda->sqlvar[1].sqlind = &flags[1];
    session->sqlda->sqlvar[1].sqllen = actual_len(product->name, sizeof(product->name));

    session->sqlda->sqlvar[2].sqldata = (ISC_SCHAR*)&product->coef;
    session->sqlda->sqlvar[2].sqlind = &flags[2];

    session->sqlda->sqlvar[3].sqldata = (ISC_SCHAR*)&product->measure;
    session->sqlda->sqlvar[3].sqlind = &flags[3];

    session->sqlda->sqlvar[4].sqldata = (ISC_SCHAR*)&product->price;
    session->sqlda->sqlvar[4].sqlind = &flags[4];

    session->sqlda->sqlvar[5].sqldata = (ISC_SCHAR*)&product->quant_limit;
    session->sqlda->sqlvar[5].sqlind = &flags[5];

    session->sqlda->sqlvar[6].sqldata = (ISC_SCHAR*)&product->unpack_measure;
    session->sqlda->sqlvar[6].sqlind = &flags[6];

    session->sqlda->sqlvar[7].sqldata = product->code;
    session->sqlda->sqlvar[7].sqlind = &flags[7];
    session->sqlda->sqlvar[7].sqllen = actual_len(product->code, sizeof(product->code));

    if (isc_dsql_execute(status, &session->alias->trans, &session->stmt, SQL_DIALECT_V6, session->sqlda)) {
        fb_log(status, "Execute add product");
        return 0;
    }
    return 1;
}
DO_EXPORT do_list_t *artix_alias_get_products_by_name(artix_alias_t *alias, const char *name_filter)
{
    ISC_STATUS_ARRAY status;
    isc_tr_handle  trans = alias->trans;
    if (!alias->trans)
        if (isc_start_transaction(status, &trans, 1, &alias->db, 0, NULL)) {
            fb_log(status, "Start transaction");
            return NULL;
        }

    isc_stmt_handle  stmt = (isc_stmt_handle)0;
    if (isc_dsql_allocate_statement(status, &alias->db, &stmt)) {
        fb_log(status, "Allocate statement");
        isc_commit_transaction(status, &trans);
        return NULL;
    }
    ISC_SHORT flag[3] = {0,0,0};
    XSQLDA *sqlda = (XSQLDA *) do_malloc(XSQLDA_LENGTH(3));
    int result = 1;
    char  selstr[BUFLEN];
    ISC_STATUS fetch_stat = 0;
    artix_product_by_name_t product;
    do_list_t *retval;

    fetch_stat = 0;
    sqlda->sqln = 3;
    sqlda->version = 1;
    retval = do_list_new(1);

    if (strlen(name_filter)) {
        char *name;
        name = iconv_(alias->tartix_conv, name_filter);
        sprintf(selstr, "select code,name,price from tmc_by_name where name like \'%s\' order by name", name);
        do_free(name);
    }
    else
        sprintf(selstr, "select code,name,price from tmc_by_name order by name");


    if (isc_dsql_prepare(status, &trans, &stmt, 0, selstr, SQL_DIALECT_V6, sqlda)) {
        fb_log(status, "Prepare \'%s\'", selstr);
        result = 0;
    }

    if (isc_dsql_execute(status, &trans, &stmt, SQL_DIALECT_V6, NULL)) {
        fb_log(status, "Execute \'%s\'", selstr);
        result = 0;
    }
    sqlda->sqlvar[0].sqlind  = &flag[0];
    sqlda->sqlvar[0].sqltype = SQL_TEXT;
    sqlda->sqlvar[0].sqllen  = ARTIX_FIELD_PRODUCT_CODE_SIZE;
    sqlda->sqlvar[1].sqlind  = &flag[1];
    sqlda->sqlvar[1].sqltype = SQL_TEXT;
    sqlda->sqlvar[1].sqllen  = ARTIX_FIELD_PRODUCT_NAME_SIZE;
    sqlda->sqlvar[2].sqlind  = &flag[2];
    sqlda->sqlvar[2].sqltype = SQL_DOUBLE;
    sqlda->sqlvar[2].sqllen  = sizeof(double);

    while (result) {

        memset(&flag, 0, sizeof(flag));

        sqlda->sqlvar[0].sqldata = product.code;
        sqlda->sqlvar[1].sqldata = product.name;
        sqlda->sqlvar[2].sqldata = (ISC_SCHAR*)&product.price;

        if ((fetch_stat = isc_dsql_fetch(status, &stmt, SQL_DIALECT_V6, sqlda)) != 0)  {
            if (fetch_stat != 100L) {
                fb_log(status, "Fetch from %s", selstr);
                result = 0;
            }
            break;
        }
        do_list_add_alloc(retval, &product, sizeof(product));
    }
    if (isc_dsql_free_statement(status, &stmt, DSQL_close)) {
        fb_log(status, "Free statement");
        result = 0;
    }

    if (!alias->trans)
        if (isc_commit_transaction(status, &trans)) {
            fb_log(status, "Commit transaction");
            result = 0;
        }

    do_free(sqlda);
    if (!result) {
        do_list_free(retval);
        return NULL;
    }
    return retval;
}

DO_EXPORT int artix_alias_get_products(artix_alias_t *alias, artix_product_t **products, int *count, int sort_by_bcode)
{
    ISC_STATUS_ARRAY status;
    isc_tr_handle  trans = alias->trans;
    if (!alias->trans)
        if (isc_start_transaction(status, &trans, 1, &alias->db, 0, NULL)) {
            fb_log(status, "Start transaction");
            return 0;
        }

    isc_stmt_handle  stmt = (isc_stmt_handle)0;
    if (isc_dsql_allocate_statement(status, &alias->db, &stmt)) {
        fb_log(status, "Allocate statement");
        isc_commit_transaction(status, &trans);
        return 0;
    }

    ISC_SHORT flag[7] = {0,0,0,0,0,0,0};
    XSQLDA *sqlda = (XSQLDA *) do_malloc(XSQLDA_LENGTH(12+1));
    int result = 1;
    char  selstr[BUFLEN];
    ISC_STATUS fetch_stat = 0;

    if (!(*count)) {

        sqlda->sqln = 1;
        sqlda->version = 1;
        sprintf(selstr, "SELECT count(BCODE) FROM TMC");

        if (isc_dsql_prepare(status, &trans, &stmt, 0, selstr, SQL_DIALECT_V6, sqlda)) {
            fb_log(status, "Prepare \'%s\'", selstr);
            result = 0;
        }

        sqlda->sqlvar[0].sqldata = (ISC_SCHAR*)count;
        sqlda->sqlvar[0].sqlind  = &flag[0];
        sqlda->sqlvar[0].sqltype = SQL_LONG + 1;

        if (result && isc_dsql_execute(status, &trans, &stmt, SQL_DIALECT_V6, NULL)) {
            fb_log(status, "Execute \'%s\'", selstr);
            result = 0;
        }
        if (result && ((fetch_stat = isc_dsql_fetch(status, &stmt, SQL_DIALECT_V6, sqlda)) != 0))  {
            if (fetch_stat != 100L)
                fb_log(status, "Fetch from %s", selstr);
            result = 0;
        }
        if (result && (*count))
           *products = (artix_product_t*)do_malloc(sizeof(artix_product_t)*(*count));
        flag[0] = 0;
    }
    if (isc_dsql_free_statement(status, &stmt, DSQL_close)) {
        fb_log(status, "Free statement");
        result = 0;
    }
    stmt = (isc_stmt_handle)0;
    if (isc_dsql_allocate_statement(status, &alias->db, &stmt)) {
        fb_log(status, "Allocate statement");
        isc_commit_transaction(status, &trans);
        return 0;
    }
    fetch_stat = 0;
    int indx;
    sqlda->sqln = 7;
    sqlda->version = 1;
    char tara_default[100];
    sprintf(selstr, "select bcode,name,measure,cquant,tara_default,price,code from tmc");
    if (sort_by_bcode)
        strcat(selstr, " order by bcode");

    if (isc_dsql_prepare(status, &trans, &stmt, 0, selstr, SQL_DIALECT_V6, sqlda)) {
        fb_log(status, "Prepare \'%s\'", selstr);
        result = 0;
    }

    if (isc_dsql_execute(status, &trans, &stmt, SQL_DIALECT_V6, NULL)) {
        fb_log(status, "Execute \'%s\'", selstr);
        result = 0;
    }
    sqlda->sqlvar[0].sqlind  = &flag[0];
    sqlda->sqlvar[0].sqltype = SQL_TEXT;
    sqlda->sqlvar[0].sqllen  = ARTIX_FIELD_BCODE_SIZE;
    sqlda->sqlvar[1].sqlind  = &flag[1];
    sqlda->sqlvar[1].sqltype = SQL_TEXT;
    sqlda->sqlvar[1].sqllen  = ARTIX_FIELD_PRODUCT_NAME_SIZE;
    sqlda->sqlvar[2].sqlind  = &flag[2];
    sqlda->sqlvar[2].sqltype = SQL_LONG;
    sqlda->sqlvar[2].sqllen  = sizeof(int);
    sqlda->sqlvar[3].sqlind  = &flag[3];
    sqlda->sqlvar[3].sqltype = SQL_DOUBLE;
    sqlda->sqlvar[3].sqllen  = sizeof(double);
    sqlda->sqlvar[4].sqlind  = &flag[4];
    sqlda->sqlvar[4].sqltype = SQL_TEXT;
    sqlda->sqlvar[4].sqllen  = sizeof(tara_default);
    sqlda->sqlvar[5].sqlind  = &flag[5];
    sqlda->sqlvar[5].sqltype = SQL_DOUBLE;
    sqlda->sqlvar[5].sqllen  = sizeof(double);
    sqlda->sqlvar[6].sqlind  = &flag[6];
    sqlda->sqlvar[6].sqltype = SQL_TEXT;
    sqlda->sqlvar[6].sqllen  = ARTIX_FIELD_PRODUCT_CODE_SIZE;

    for (indx = 0; indx < *count && result && !fetch_stat; indx++) {

        memset(&flag, 0, sizeof(flag));

        sqlda->sqlvar[0].sqldata = (*products)[indx].bcode;
        sqlda->sqlvar[1].sqldata = (*products)[indx].name;
        sqlda->sqlvar[2].sqldata = (ISC_SCHAR*)&(*products)[indx].measure;
        sqlda->sqlvar[3].sqldata = (ISC_SCHAR*)&(*products)[indx].coef;
        sqlda->sqlvar[4].sqldata = (ISC_SCHAR*)&tara_default;
        sqlda->sqlvar[5].sqldata = (ISC_SCHAR*)&(*products)[indx].price;
        sqlda->sqlvar[6].sqldata = (*products)[indx].code;

        if ((fetch_stat = isc_dsql_fetch(status, &stmt, SQL_DIALECT_V6, sqlda)) != 0)  {
            if (fetch_stat == 100L) {
                *count = indx + 1;
            }
            else {
                fb_log(status, "Fetch from %s", selstr);
                result = 0;
            }
            break;
        }
        (*products)[indx].unpack_measure = atoi(tara_default);
    }
    if (isc_dsql_free_statement(status, &stmt, DSQL_close)) {
        fb_log(status, "Free statement");
        result = 0;
    }

    if (!alias->trans)
        if (isc_commit_transaction(status, &trans)) {
            fb_log(status, "Commit transaction");
            result = 0;
        }

    do_free(sqlda);
    return result;
}

//local function
static void fb_log(ISC_STATUS* status, const char* cformat, ...)
{
    va_list ap;
    int sz = 1024, n = -1;
    char *buf = NULL;
    while (n == -1) {
        buf = (char*) do_realloc(buf, sz + 1);
        if (!buf) return;
        va_start(ap, cformat);
        n = vsnprintf(buf, sz , cformat, ap);
        va_end(ap);
        sz *=2;
    }
    do_log(LOG_ERR, "Firebird operation %s statis %d ", buf, *status);
    do_free(buf);
    sz = 1024;
    buf = (char*) do_malloc(sz + 1);
//#ifdef _WIN3211
//    while (isc_interprete(buf, &status))
//#else
    while (fb_interpret(buf, sz, (const ISC_STATUS**)&status))
//#endif
        do_log(LOG_ERR, "%s", buf);
    do_free(buf);
}

static void from_artix_init(artix_alias_t *alias, int use_utf)
{
    const char *str;
    if (use_utf)
        str = UTF_CHARSET;
    else
        str = get_locale_lang();

    alias->fartix_conv = iconv_open(str, ARTIX_CHARSET);
    if (alias->fartix_conv == (iconv_t)-1) {
        do_log(LOG_ERR, "Error iconv_open (\"%s\" to \"%s\") ", ARTIX_CHARSET, str);
        return;
    }
}

static void to_artix_init(artix_alias_t *alias, int use_utf)
{
    const char *str;
    if (use_utf)
        str = UTF_CHARSET;
    else
        str = get_locale_lang();
    alias->tartix_conv = iconv_open(ARTIX_CHARSET, str);
    if (alias->tartix_conv == (iconv_t)-1)
        do_log(LOG_ERR, "Error iconv_open (\"%s\" to \"%s\") ", str, ARTIX_CHARSET);
}
typedef union {
    int       integer;
    char     *string;
    struct tm date;
    double    real;
    void     *unsupport;
} field_value_t;

static int artix_select_exec(artix_select_t *select, artix_alias_t *alias, const char *selstr)
{
    ISC_STATUS_ARRAY status;

    isc_tr_handle  trans = alias->trans;
    if ( !alias->trans )
        if ( isc_start_transaction(status, &trans, 1, &alias->db, 0, NULL) ) {
            fb_log(status, "Start transaction");
            return 0;
        }

    isc_stmt_handle  stmt = (isc_stmt_handle)0;
    if ( isc_dsql_allocate_statement(status, &alias->db, &stmt) ) {
        fb_log(status, "Allocate statement");
        isc_commit_transaction(status, &trans);
        return 0;
    }


    XSQLDA *sqlda = (XSQLDA *) do_malloc(XSQLDA_LENGTH(100));

    sqlda->sqln = 100;
    sqlda->version = 1;
    int result = 1;
    int i;
    do_list_t *data = NULL;
    ISC_SHORT *flags = NULL;

    if ( isc_dsql_prepare(status, &trans, &stmt, 0, selstr, SQL_DIALECT_V6, sqlda) ) {
        fb_log(status, "Prepare \'%s\'", selstr);
        result = 0;
        goto end_artix_select_exec;
    }

    if ( isc_dsql_describe(status, &stmt, 1, sqlda) ) {
        fb_log(status, "Prepare \'%s\'", selstr);
        result = 0;
        goto end_artix_select_exec;
    }

    data = do_list_new(TRUE);
    flags = do_malloc0(sizeof(ISC_SHORT)*sqlda->sqld);

    for ( i = 0 ;  i < sqlda->sqld; i++ )
    {
        XSQLVAR *var = &sqlda->sqlvar[i];
        field_t *field = do_malloc0(sizeof(field_t));

        switch ( var->sqltype & ~1 ) {
            case SQL_TEXT:
            case SQL_VARYING:
                field->type = ARTIX_SELECT_TYPE_STRING;
                var->sqltype = SQL_TEXT;
                break;
            case SQL_SHORT:
            case SQL_LONG:
                field->type = ARTIX_SELECT_TYPE_INTEGER;
                var->sqltype = SQL_LONG;
                var->sqllen = sizeof(int);
                break;
            case SQL_FLOAT:
            case SQL_DOUBLE:
            case SQL_INT64:
                field->type = ARTIX_SELECT_TYPE_DOUBLE;
                var->sqltype = SQL_DOUBLE;
                var->sqllen = sizeof(double);
                break;
            case SQL_TIMESTAMP:
            case SQL_TYPE_DATE:
            case SQL_TYPE_TIME:
                field->type = ARTIX_SELECT_TYPE_TIME;
                var->sqltype = SQL_TIMESTAMP;
                var->sqllen = sizeof(ISC_TIMESTAMP);
                break;
            default:
                field->type = ARTIX_SELECT_TYPE_UNSUPPORT;
                do_log(LOG_ALERT, "unsuported type %d field %s select %s", var->sqltype & ~1, field->name, selstr);
                break;
        }

        field->len = var->sqllen;
        field->name = artix_text_(alias, var->sqlname, var->sqlname_length);
        do_list_add(data, do_malloc(field->len));
        var->sqldata = data->list[i];
        var->sqlind  = &flags[i];

        do_list_add(select->field, field);
    }

    if (isc_dsql_execute(status, &trans, &stmt, SQL_DIALECT_V6, NULL)) {
        fb_log(status, "Execute \'%s\'", selstr);
        result = 0;
        goto end_artix_select_exec;
    }

    ISC_STATUS fetch_stat = 0;

    while (result) {

        memset(flags, 0, sizeof(ISC_SHORT)*sqlda->sqld);

        if ((fetch_stat = isc_dsql_fetch(status, &stmt, SQL_DIALECT_V6, sqlda)) != 0)  {
            if (fetch_stat != 100L) {
                fb_log(status, "Fetch from %s", selstr);
                result = 0;
            }
            break;
        }
        do_list_t *values = do_list_new(TRUE);
        for (i = 0; i < select->field->count; i++) {
            field_t *field = select->field->list[i];
            field_value_t value;
            switch (field->type) {
                case ARTIX_SELECT_TYPE_STRING:
                    value.string = artix_text_(alias, data->list[i], field->len);
                    break;
                case ARTIX_SELECT_TYPE_INTEGER:
                    value.integer = *((int*)data->list[i]);
                    break;
                case ARTIX_SELECT_TYPE_DOUBLE:
                    value.real = *((double*)data->list[i]);
                    break;
                case ARTIX_SELECT_TYPE_TIME:
                    isc_decode_timestamp(data->list[i], &value.date);
                    break;
                case ARTIX_SELECT_TYPE_UNSUPPORT:
                    value.unsupport = do_malloc(field->len);
                    memcpy(value.unsupport, data->list[i], field->len);
                    break;
            }
            do_list_add_alloc(values, &value, sizeof(value));
        }
        do_list_add(select->record, values);

    }

end_artix_select_exec:
    if (isc_dsql_free_statement(status, &stmt, DSQL_close)) {
        fb_log(status, "Free statement");
        result = 0;
    }

    if (!alias->trans)
        if (isc_commit_transaction(status, &trans)) {
            fb_log(status, "Commit transaction");
            result = 0;
        }
    do_free(sqlda);
    if (data)
        do_list_free(data);
    if (flags)
        do_free(flags);
    return result;
}
DO_EXPORT artix_select_t *artix_select_new(artix_alias_t *alias, const char *str)
{
    artix_select_t *select = do_malloc(sizeof(artix_select_t));
    select->field = do_list_new(FALSE);
    select->record = do_list_new(FALSE);

    if (!artix_select_exec(select, alias, str)) {
        artix_select_free(select);
        return NULL;
    }
    return select;
}
static int rec_free(do_list_t *values, do_list_t *fields)
{
    int i;
    for (i = 0; i < fields->count; i++) {
        field_t *field = fields->list[i];
        field_value_t *value = values->list[i];
        switch (field->type) {
            case ARTIX_SELECT_TYPE_STRING:
                do_free(value->string);
                break;
            case ARTIX_SELECT_TYPE_INTEGER:
            case ARTIX_SELECT_TYPE_DOUBLE:
            case ARTIX_SELECT_TYPE_TIME:
                break;
            case ARTIX_SELECT_TYPE_UNSUPPORT:
                do_free(value->unsupport);
                break;
        }
    }
    do_list_free(values);
    return TRUE;
}
static int field_free(field_t *field, void *data)
{
    do_free(field->name);
    do_free(field);
    return TRUE;
}


DO_EXPORT void artix_select_free(artix_select_t *select)
{
    do_list_foreach(select->record, (do_list_foreach_func)rec_free, select->field);
    do_list_foreach(select->field, (do_list_foreach_func)field_free, NULL);
    do_list_free(select->field);
    do_list_free(select->record);
    do_free(select);
}
DO_EXPORT int artix_select_record_count(artix_select_t *select)
{
    return select->record->count;
}
DO_EXPORT int artix_select_field_count(artix_select_t *select)
{
    return select->field->count;
}
DO_EXPORT artix_select_type_t artix_select_field_type(artix_select_t *select, int indx)
{
    if (select->field->count <= indx)
        return ARTIX_SELECT_TYPE_UNSUPPORT;
    return ((field_t*)select->field->list[indx])->type;
}
static char empty_str[] = "";
static struct tm empty_tm = {0,};
DO_EXPORT artix_select_type_t artix_select_field_type_by_name(artix_select_t *select, const char *name)
{
    int i;
    for ( i = 0; i < select->field->count; i++ )
        if ( !strcmp(((field_t*)select->field->list[i])->name, name) )
            return ((field_t*)select->field->list[i])->type;
    return ARTIX_SELECT_TYPE_UNSUPPORT;
}
DO_EXPORT const char *artix_select_field_name(artix_select_t *select, int indx)
{
    if ( select->field->count <= indx )
        return empty_str;
    return ((field_t*)select->field->list[indx])->name;
}
DO_EXPORT int artix_select_value_integer(artix_select_t *select, int record, int indx)
{
    if ( record >= select->record->count ||
         indx >= select->field->count ||
         (((field_t*)select->field->list[indx])->type != ARTIX_SELECT_TYPE_INTEGER &&
          ((field_t*)select->field->list[indx])->type != ARTIX_SELECT_TYPE_DOUBLE ) )
        return 0;
    if ( ((field_t*)select->field->list[indx])->type == ARTIX_SELECT_TYPE_INTEGER )
        return ((field_value_t*)
                ((do_list_t*)select->record->list[record])->list[indx])->integer;
    int res;
    res = ((field_value_t*)
                ((do_list_t*)select->record->list[record])->list[indx])->real;
    return res;
}
DO_EXPORT const char *artix_select_value_string(artix_select_t *select, int record, int indx)
{
    if ( record >= select->record->count ||
         indx >= select->field->count ||
         ((field_t*)select->field->list[indx])->type != ARTIX_SELECT_TYPE_STRING )
        return empty_str;
    return ((field_value_t*)
                ((do_list_t*)select->record->list[record])->list[indx])->string;
}
DO_EXPORT double artix_select_value_double(artix_select_t *select, int record, int indx)
{
    if ( record >= select->record->count ||
         indx >= select->field->count ||
         (((field_t*)select->field->list[indx])->type != ARTIX_SELECT_TYPE_INTEGER &&
          ((field_t*)select->field->list[indx])->type != ARTIX_SELECT_TYPE_DOUBLE ) )
        return 0;
    if ( ((field_t*)select->field->list[indx])->type == ARTIX_SELECT_TYPE_DOUBLE )
        return ((field_value_t*)
                ((do_list_t*)select->record->list[record])->list[indx])->real;
    double res;
    res = ((field_value_t*)
                ((do_list_t*)select->record->list[record])->list[indx])->integer;
    return res;
}
DO_EXPORT struct tm *artix_select_value_time(artix_select_t *select, int record, int indx)
{
    if ( record >= select->record->count ||
         indx >= select->field->count ||
         ((field_t*)select->field->list[indx])->type != ARTIX_SELECT_TYPE_TIME )
        return &empty_tm;
    return &((field_value_t*)
                ((do_list_t*)select->record->list[record])->list[indx])->date;
}
static int artix_select_get_by_name(artix_select_t *select, const char *name)
{
    int i;
    for ( i = 0; i < select->field->count; i++ )
        if ( !strcmp(((field_t*)select->field->list[i])->name, name) )
            return i;
    return -1;
}
DO_EXPORT int artix_select_value_by_name_integer(artix_select_t *select, int record, const char *name)
{
    int indx;
    indx = artix_select_get_by_name(select, name);
    if (indx == -1)
        return 0;
    return artix_select_value_integer(select, record, indx);
}
DO_EXPORT const char *artix_select_value_by_name_string(artix_select_t *select, int record, const char *name)
{
    int indx;
    indx = artix_select_get_by_name(select, name);
    if (indx == -1)
        return empty_str;
    return artix_select_value_string(select, record, indx);
}

DO_EXPORT double artix_select_value_by_name_double(artix_select_t *select, int record, const char *name)
{
    int indx;
    indx = artix_select_get_by_name(select, name);
    if (indx == -1)
        return 0;
    return artix_select_value_double(select, record, indx);
}
DO_EXPORT struct tm *artix_select_value_by_name_time(artix_select_t *select, int record, const char *name)
{
    int indx;
    indx = artix_select_get_by_name(select, name);
    if (indx == -1)
        return &empty_tm;
    return artix_select_value_time(select, record, indx);
}
DO_EXPORT int artix_check_product(artix_alias_t *alias, artix_product_t *product)
{
    ISC_STATUS_ARRAY status;
    isc_tr_handle  trans = alias->trans;
    if (!alias->trans)
        if (isc_start_transaction(status, &trans, 1, &alias->db, 0, NULL)) {
            fb_log(status, "Start transaction");
            return FALSE;
        }

    isc_stmt_handle  stmt = (isc_stmt_handle)0;
    if (isc_dsql_allocate_statement(status, &alias->db, &stmt)) {
        fb_log(status, "Allocate statement");
        isc_commit_transaction(status, &trans);
        return FALSE;
    }
    ISC_SHORT flag[1] = {0};
    XSQLDA *sqlda = (XSQLDA *) do_malloc(XSQLDA_LENGTH(1));
    int result = TRUE;
    char  selstr[BUFLEN];
    ISC_STATUS fetch_stat = 0;

    fetch_stat = 0;
    sqlda->sqln = 1;
    sqlda->version = 1;

    char *bcode;
    bcode = artix_text(alias, product->bcode);
    sprintf(selstr, "select price from tmc where bcode=\'%s\'", bcode);
    do_free(bcode);

    if (isc_dsql_prepare(status, &trans, &stmt, 0, selstr, SQL_DIALECT_V6, sqlda)) {
        fb_log(status, "Prepare \'%s\'", selstr);
        result = FALSE;
    }

    if (isc_dsql_execute(status, &trans, &stmt, SQL_DIALECT_V6, NULL)) {
        fb_log(status, "Execute \'%s\'", selstr);
        result = FALSE;
    }
    if ( result ) {
	double price;
	sqlda->sqlvar[0].sqlind  = &flag[0];
	sqlda->sqlvar[0].sqltype = SQL_DOUBLE;
	sqlda->sqlvar[0].sqllen  = sizeof(double);
        memset(&flag, 0, sizeof(flag));

        sqlda->sqlvar[0].sqldata = (ISC_SCHAR*)&price;

        if ( (fetch_stat = isc_dsql_fetch(status, &stmt, SQL_DIALECT_V6, sqlda)) != 0 )  {
     	    result = FALSE;
     	    bcode = artix_text(alias, product->bcode);
            do_log(LOG_INFO, "\"%s\" not found", bcode);
            do_free(bcode);
            if ( fetch_stat != 100L )
                fb_log(status, "Fetch from %s", selstr);
        }
        if ( result ) {
          if ( price < product->price - 0.01 || price > product->price + 0.01 ) {
       	  bcode = artix_text(alias, product->bcode);
        do_log(LOG_INFO, "\"%s\" %f %f", bcode, price, product->price);
        do_free(bcode);
      	    result = FALSE;
    	    }
    	}
    }
    if ( isc_dsql_free_statement(status, &stmt, DSQL_close) ) {
        fb_log(status, "Free statement");
        result = FALSE;
    }

    if (!alias->trans)
        if (isc_commit_transaction(status, &trans)) {
            fb_log(status, "Commit transaction");
            result = FALSE;
        }

    do_free(sqlda);
    return result;
}
