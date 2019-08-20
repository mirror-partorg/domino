#include <domino.h>
#include <stddef.h>
#include "../../misc/define.h"

typedef struct PACK_STRUCT {
    BTI_LONG date;
    BTI_BYTE sums[3][8];
} prowod_ret_t;

DO_EXPORT int do_balance_get_key_from_prowod(do_alias_t *alias, prowod_struct_t *prowod_st, do_balance_key_t *key, do_balance_side_t side, int *err)
{
    account_rec_t account;
    subaccount_rec_t subaccount;
    int status;

    if (side == DO_BALANCE_SIDE_DEBIT)
        memcpy(&account.data.account, prowod_st->debit_account, sizeof(account.data.account));
    else
        memcpy(&account.data.account, prowod_st->credit_account, sizeof(account.data.account));
    account.data.itog = 0;

    status = do_alias_cache_account(alias, &account);
    if (status == DO_KEY_NOT_FOUND) {
        char *acode;
        acode = do_text(alias, account.data.account);
        do_log(LOG_ERR, "account \"%s\" not found", acode);
        do_free(acode);
    }
    if (status != DO_OK) {
        *err = 1;
        return FALSE;
    }
    if (do_text_is_empty(account.data.name))
        return FALSE;

    if (account.data.analitsub) {
        if (side == DO_BALANCE_SIDE_DEBIT) {
            memcpy(&subaccount.data.account, prowod_st->debit_account, sizeof(subaccount.data.account));
            memcpy(&subaccount.data.subaccount, prowod_st->debit_subaccount, sizeof(subaccount.data.subaccount));
        }
        else {
            memcpy(&subaccount.data.account, prowod_st->credit_account, sizeof(subaccount.data.account));
            memcpy(&subaccount.data.subaccount, prowod_st->credit_subaccount, sizeof(subaccount.data.subaccount));
        }
        subaccount.data.itog = 0;

        status = do_alias_cache_subaccount(alias, &subaccount);
        if (status == DO_KEY_NOT_FOUND) {
            char *acode;
            char *acode1;
            acode = do_text(alias, subaccount.data.account);
            acode1 = do_text(alias, subaccount.data.subaccount);
            do_log(LOG_ERR, "subaccount \"%s.%s\" not found", acode, acode1);
            do_free(acode);
        }
        if (status != DO_OK) {
            *err = 1;
            return FALSE;
        }
    }

    memcpy(&key->division, prowod_st->division, sizeof(key->division));
    if (side == DO_BALANCE_SIDE_DEBIT) {
        memcpy(&key->account, &prowod_st->debit_account, sizeof(key->account));
        if (account.data.analitsub)
            memcpy(&key->subaccount, &prowod_st->debit_subaccount, sizeof(key->subaccount));
        else
            do_text_set(alias, key->subaccount, "");
        if (account.data.analitpartner) {
            memcpy(&key->p_g_code, &prowod_st->debit_p_g_code, sizeof(key->p_g_code));
            key->p_code = prowod_st->debit_p_code;
        }
        else {
            do_text_set(alias, key->p_g_code, "");
            key->p_code = 0;
        }
        if (account.data.analitother)
            memcpy(&key->other, &prowod_st->debit_other, sizeof(key->other));
        else
            do_text_set(alias, key->other, "");
    }
    else {
        memcpy(&key->account, &prowod_st->credit_account, sizeof(key->account));
        if (account.data.analitsub)
            memcpy(&key->subaccount, &prowod_st->credit_subaccount, sizeof(key->subaccount));
        else
            do_text_set(alias, key->subaccount, "");
        if (account.data.analitpartner) {
            memcpy(&key->p_g_code, &prowod_st->credit_p_g_code, sizeof(key->p_g_code));
            key->p_code = prowod_st->credit_p_code;
        }
        else {
            do_text_set(alias, key->p_g_code, "");
            key->p_code = 0;
        }
        if (account.data.analitother)
            memcpy(&key->other, &prowod_st->credit_other, sizeof(key->other));
        else
            do_text_set(alias, key->other, "");
    }
    if (side == DO_BALANCE_SIDE_DEBIT) {
        key->debit_sum[0] = do_pdecimal(prowod_st->sums[0], 8, 2);
        key->debit_sum[1] = do_pdecimal(prowod_st->sums[1], 8, 2);
        key->debit_sum[2] = do_pdecimal(prowod_st->sums[2], 8, 2);
        key->credit_sum[0] = 0;
        key->credit_sum[1] = 0;
        key->credit_sum[2] = 0;
    }
    else {
        key->credit_sum[0] = do_pdecimal(prowod_st->sums[0], 8, 2);
        key->credit_sum[1] = do_pdecimal(prowod_st->sums[1], 8, 2);
        key->credit_sum[2] = do_pdecimal(prowod_st->sums[2], 8, 2);
        key->debit_sum[0] = 0;
        key->debit_sum[1] = 0;
        key->debit_sum[2] = 0;
    }
    key->date = prowod_st->date;
    return TRUE;

}
static do_ret_list_t *get_prowod_by_key(do_alias_t *alias, do_balance_key_t *key, do_balance_side_t side, account_rec_t *account, do_extended_break_func break_func)
{
    prowod_key2_t prowod_key2;
    do_extended_t *extended;
    do_ret_list_t *list;
    int count;

    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_PROWOD);
    do_extended_set_key(extended, (side == DO_BALANCE_SIDE_DEBIT) ? 2 : 3, &prowod_key2);

    if (!account->data.analitother) {
        if (!account->data.analitpartner) {
            if (!account->data.analitsub)
                count = 2;
            else
                count = 3;
        }
        else
            count = 5;
    }
    else
        count = 6;

    do_extended_set_filter(extended, TRUE, count,
                           sizeof(prowod_key2.division),
                           sizeof(prowod_key2.debit_account),
                           sizeof(prowod_key2.debit_subaccount),
                           sizeof(prowod_key2.debit_p_g_code),
                           sizeof(prowod_key2.debit_p_code),
                           sizeof(prowod_key2.debit_other)
                           );

    memcpy(&prowod_key2.division, &key->division, sizeof(key->division));
    memcpy(&prowod_key2.debit_account, &key->account, sizeof(key->account));
    do_extended_set_filter_term(extended, 0,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(prowod_struct_t, division),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_AND,
                                &prowod_key2.division);
    do_extended_set_filter_term(extended, 1,
                                DO_EXTENDED_TYPE_STRING,
                                (side == DO_BALANCE_SIDE_DEBIT) ?
                                 offsetof(prowod_struct_t, debit_account) :
                                 offsetof(prowod_struct_t, credit_account),
                                DO_EXTENDED_COMP_EQUAL,
                                (count > 2) ? DO_EXTENDED_CONNECTION_AND : DO_EXTENDED_CONNECTION_NO,
                                &prowod_key2.debit_account);
    if (count > 2) {
        memcpy(&prowod_key2.debit_subaccount, &key->subaccount, sizeof(key->subaccount));
        do_extended_set_filter_term(extended, 2,
                                    DO_EXTENDED_TYPE_STRING,
                                    (side == DO_BALANCE_SIDE_DEBIT) ?
                                     offsetof(prowod_struct_t, debit_subaccount) :
                                     offsetof(prowod_struct_t, credit_subaccount),
                                    DO_EXTENDED_COMP_EQUAL,
                                    (count > 3) ? DO_EXTENDED_CONNECTION_AND : DO_EXTENDED_CONNECTION_NO,
                                    &prowod_key2.debit_subaccount);
    }
    else
        do_text_set_empty(prowod_key2.debit_subaccount);
    if (count > 3) {
        memcpy(&prowod_key2.debit_p_g_code, &key->p_g_code, sizeof(key->p_g_code));
        prowod_key2.debit_p_code = key->p_code;
        do_extended_set_filter_term(extended, 3,
                                    DO_EXTENDED_TYPE_STRING,
                                    (side == DO_BALANCE_SIDE_DEBIT) ?
                                     offsetof(prowod_struct_t, debit_p_g_code) :
                                     offsetof(prowod_struct_t, credit_p_g_code),
                                    DO_EXTENDED_COMP_EQUAL,
                                    DO_EXTENDED_CONNECTION_AND,
                                    &prowod_key2.debit_p_g_code);
        do_extended_set_filter_term(extended, 4,
                                    DO_EXTENDED_TYPE_INTEGER,
                                    (side == DO_BALANCE_SIDE_DEBIT) ?
                                     offsetof(prowod_struct_t, debit_p_code) :
                                     offsetof(prowod_struct_t, credit_p_code),
                                    DO_EXTENDED_COMP_EQUAL,
                                    (count > 5) ? DO_EXTENDED_CONNECTION_AND : DO_EXTENDED_CONNECTION_NO,
                                    &prowod_key2.debit_p_code);
    }
    else {
        do_text_set_empty(prowod_key2.debit_p_g_code);
        prowod_key2.debit_p_code = 0;
    }
    if (count > 5) {
        memcpy(&prowod_key2.debit_other, &key->other, sizeof(key->other));
        do_extended_set_filter_term(extended, 5,
                                    DO_EXTENDED_TYPE_STRING,
                                    (side == DO_BALANCE_SIDE_DEBIT) ?
                                     offsetof(prowod_struct_t, debit_other) :
                                     offsetof(prowod_struct_t, credit_other),
                                    DO_EXTENDED_COMP_EQUAL,
                                    DO_EXTENDED_CONNECTION_NO,
                                    &prowod_key2.debit_other);
    }
    else
        do_text_set_empty(prowod_key2.debit_other);
    prowod_key2.date = 0;
    do_extended_set_fields_full_record(extended, sizeof(prowod_struct_t));

    do_extended_set_fields(extended, 2,
                           do_size_and_offset(prowod_struct_t, date),
                           do_size_and_offset(prowod_struct_t, sums));
    list = do_extended_get(extended, -1);
    do_extended_free(extended);
    return list;

}
DO_EXPORT int do_balance_rebuild(do_alias_t *alias, do_balance_key_t *key, do_extended_break_func break_func)
{
    char *aname;
    char *sname;
    char *p_gcode;
    char *other;
    int err = 0, i, j, update;
    do_ret_list_t *list;
    prowod_ret_t *rec;

    aname = do_text(alias, key->account);
    sname = do_text(alias, key->subaccount);
    p_gcode = do_text(alias, key->p_g_code);
    other = do_text(alias, key->other);
    do_log(LOG_INFO, "rebuild balance \"%s.%s %s.%d %s\"", aname, sname, p_gcode, key->p_code, other);
    do_free(aname);do_free(sname);do_free(p_gcode);do_free(other);

    account_rec_t account;
    subaccount_rec_t subaccount;
    double sum_debit[3] = {0,0,0};
    double sum_credit[3] = {0,0,0};
    double sum;
    int    sumincl[3] = {1,0,0};
    int status;
    BTI_LONG date_e = 0, date_n = 0;

    memcpy(&account.data.account, &key->account, sizeof(account.data.account));
    account.data.itog = 0;
    status = do_alias_cache_account(alias, &account);
    if (status == DO_KEY_NOT_FOUND) {
        char *acode;
        acode = do_text(alias, account.data.account);
        do_log(LOG_ERR, "account \"%s\" - not found", acode);
        do_free(acode);
    }
    if (status != DO_OK)
        return FALSE;

    if (account.data.analitsub) {
        memcpy(&subaccount.data.account, &key->account, sizeof(subaccount.data.account));
        memcpy(&subaccount.data.subaccount, &key->subaccount, sizeof(subaccount.data.subaccount));
        subaccount.data.itog = 0;
        status = do_alias_cache_subaccount(alias, &subaccount);
        if (status == DO_KEY_NOT_FOUND) {
            char *acode;
            acode = do_text(alias, subaccount.data.subaccount);
            do_log(LOG_ERR, "subaccount \"%s\" - not found", acode);
            do_free(acode);
        }
        if (status != DO_OK)
            return FALSE;
        sumincl[1] = account.data.sumincl[1] || subaccount.data.sumincl[1] ;
        sumincl[2] = account.data.sumincl[2] || subaccount.data.sumincl[2] ;
    }
    else {
        sumincl[1] = account.data.sumincl[1];
        sumincl[2] = account.data.sumincl[2];
    }

    if (do_alias_tran_begin(alias) != DO_OK)
        return FALSE;

    if ((list = get_prowod_by_key(alias, key, DO_BALANCE_SIDE_DEBIT, &account, break_func)) != NULL) {
        for (i = 0; i < do_ret_list_count(list); i++) {
            rec = do_ret_list_item_data(list, i);
            if (!date_n || date_n > rec->date)
                date_n = rec->date;
            if (!date_e || date_e < rec->date)
                date_e = rec->date;
            for(j = 0; j < 3; j ++)
                if (sumincl[j])
                    sum_debit[j] +=
                      do_pdecimal(rec->sums[j], 8, 2);
        }
        do_ret_list_free(list);

    }
    else
        err = 1;
    if (!err && (list = get_prowod_by_key(alias, key, DO_BALANCE_SIDE_CREDIT, &account, break_func)) != NULL) {
        for (i = 0; i < do_ret_list_count(list); i++) {
            rec = do_ret_list_item_data(list, i);
            if (!date_n || date_n > rec->date)
                date_n = rec->date;
            if (!date_e || date_e < rec->date)
                date_e = rec->date;
            for(j = 0; j < 3; j ++)
                if (sumincl[j])
                    sum_credit[j] +=
                      do_pdecimal(rec->sums[j], 8, 2);
        }
        do_ret_list_free(list);

    }
    else
        err = 1;

    if (!err) {
        for (j = 0; j < 3; j++) {
            if (sum_debit[j] < 0) {
                sum_credit[j] -= sum_debit[j];
                sum_debit[j] = 0;
            }
            if (sum_credit[j] < 0) {
                sum_debit[j] -= sum_credit[j];
                sum_credit[j] = 0;
            }
            if( (!account.data.analitsub && account.data.positivesaldo   ) ||
                ( account.data.analitsub && subaccount.data.positivesaldo) ) {
                if (sum_debit[j] > sum_credit[j]) {
                    sum_debit[j] -= sum_credit[j];
                    sum_credit[j] = 0;
                }
                else {
                    sum_credit[j] -= sum_debit[j];
                    sum_debit[j] = 0;
                }
            }
            if ( (!account.data.analitsub && (!account.data.debit || !account.data.credit)) ||
                 ( account.data.analitsub && (!subaccount.data.debit || !subaccount.data.credit)) ) {
                if ( (!account.data.analitsub && account.data.debit) ||
                     ( account.data.analitsub && subaccount.data.debit) ) {
                    sum_debit[j] -= sum_credit[j];
                    sum_credit[j] = 0;
                }
                else {
                    sum_credit[j] -= sum_debit[j];
                    sum_debit[j] = 0;
                }
            }
        }

        saldo_key0_t saldo_key0;
        saldo_rec_t saldo;
        memcpy(&saldo_key0.division, &key->division, sizeof(saldo_key0.division));
        do_text_set_empty(saldo_key0.layer);
        memcpy(&saldo_key0.account, &key->account, sizeof(saldo_key0.account));
        saldo_key0.account_total = 0;
        if (account.data.analitsub)
            memcpy(&saldo_key0.subaccount, &key->subaccount, sizeof(saldo_key0.subaccount));
        else
            do_text_set_empty(saldo_key0.subaccount);
        saldo_key0.subaccount_total = 0;
        if (account.data.analitpartner) {
            memcpy(&saldo_key0.p_g_code, &key->p_g_code, sizeof(saldo_key0.p_g_code));
            saldo_key0.p_code = key->p_code;
        }
        else {
            do_text_set_empty(saldo_key0.p_g_code);
            saldo_key0.p_code = 0;
        }
        if (account.data.analitother)
            memcpy(&saldo_key0.other, &key->other, sizeof(saldo_key0.other));
        else
            do_text_set_empty(saldo_key0.other);

        status = do_saldo_get0(alias, &saldo, &saldo_key0, DO_GET_EQUAL);
        if (status == DO_ERROR)
            err = 1;
        else {
            if (status == DO_KEY_NOT_FOUND) {
                memcpy(&saldo.data.division, &saldo_key0.division, sizeof(saldo_key0.division));
                memcpy(&saldo.data.layer, &saldo_key0.layer, sizeof(saldo_key0.layer));
                memcpy(&saldo.data.account, &saldo_key0.account, sizeof(saldo_key0.account));
                memcpy(&saldo.data.account_total, &saldo_key0.account_total, sizeof(saldo_key0.account_total));
                memcpy(&saldo.data.subaccount, &saldo_key0.subaccount, sizeof(saldo_key0.subaccount));
                memcpy(&saldo.data.subaccount_total, &saldo_key0.subaccount_total, sizeof(saldo_key0.subaccount_total));
                memcpy(&saldo.data.p_g_code, &saldo_key0.p_g_code, sizeof(saldo_key0.p_g_code));
                memcpy(&saldo.data.p_code, &saldo_key0.p_code, sizeof(saldo_key0.p_code));
                memcpy(&saldo.data.other, &saldo_key0.other, sizeof(saldo_key0.other));
                update = 1;
            }
            else {
                update = 0;
                for (j = 0; j < 3; j++) {
                    sum = do_pdecimal(saldo.data.debit_sum[j], 8, 2);
                    if (sum < sum_debit[j] - 0.001 || sum > sum_debit[j] + 0.001) {
                        update = 1;
                        break;
                    }
                    sum = do_pdecimal(saldo.data.credit_sum[j], 8, 2);
                    if (sum < sum_credit[j] - 0.001 || sum > sum_credit[j] + 0.001) {
                        update = 1;
                        break;
                    }
                }
            }
            if (update) {
                saldo.data.date_n = date_n;
                saldo.data.date = date_e;
                saldo.data.date_modify = 0;
                saldo.data.time_modify = 0;
                for (j = 0; j < 3; j++) {
                    do_pdecimal_set(saldo.data.debit_sum[j], 8, 2, sum_debit[j]);
                    do_pdecimal_set(saldo.data.credit_sum[j], 8, 2, sum_credit[j]);
                }
                if (status == DO_KEY_NOT_FOUND)
                    err = do_saldo_insert(alias, &saldo) != DO_OK;
                else
                    err = do_saldo_update(alias, &saldo) != DO_OK;
            }
        }
    }
    if (!err)
        err = do_alias_tran_end(alias) != DO_OK;
    else
        do_alias_tran_abort(alias);
    return !err;
}

DO_EXPORT int do_balance_add(do_alias_t *alias, do_balance_key_t *key, int accept, do_extended_break_func break_func)
{
    char *aname;
    char *sname;
    char *p_gcode;
    char *other;
    int err = 0, j;

    aname = do_text(alias, key->account);
    sname = do_text(alias, key->subaccount);
    p_gcode = do_text(alias, key->p_g_code);
    other = do_text(alias, key->other);
    do_free(aname);do_free(sname);do_free(p_gcode);do_free(other);

    account_rec_t account;
    subaccount_rec_t subaccount;
    double sum_debit[3] = {0,0,0};
    double sum_credit[3] = {0,0,0};
    int    sumincl[3] = {1,0,0};
    int status;
    BTI_LONG date_e = 0, date_n = 0;
    saldo_key0_t saldo_key0;
    saldo_rec_t saldo;

    do_cpy(account.data.account, key->account);
    account.data.itog = 0;
    status = do_alias_cache_account(alias, &account);
    if (status == DO_KEY_NOT_FOUND) {
        char *acode;
        acode = do_text(alias, account.data.account);
        do_log(LOG_ERR, "account \"%s\" - not found", acode);
        do_free(acode);
    }
    if (status != DO_OK)
        return FALSE;

    if (account.data.analitsub) {
        do_cpy(subaccount.data.account, key->account);
        do_cpy(subaccount.data.subaccount, key->subaccount);
        subaccount.data.itog = 0;
        status = do_alias_cache_subaccount(alias, &subaccount);
        if (status == DO_KEY_NOT_FOUND) {
            char *acode;
            acode = do_text(alias, subaccount.data.subaccount);
            do_log(LOG_ERR, "subaccount \"%s\" - not found", acode);
            do_free(acode);
        }
        if (status != DO_OK)
            return FALSE;
        sumincl[1] = account.data.sumincl[1] || subaccount.data.sumincl[1] ;
        sumincl[2] = account.data.sumincl[2] || subaccount.data.sumincl[2] ;
    }
    else {
        sumincl[1] = account.data.sumincl[1];
        sumincl[2] = account.data.sumincl[2];
    }

    if (do_alias_tran_begin(alias) != DO_OK)
        return FALSE;

    do_cpy(saldo_key0.division, key->division);
    do_text_set_empty(saldo_key0.layer);
    do_cpy(saldo_key0.account, key->account);
    saldo_key0.account_total = 0;
    if (account.data.analitsub)
        do_cpy(saldo_key0.subaccount, key->subaccount)
    else
        do_text_set_empty(saldo_key0.subaccount);
    saldo_key0.subaccount_total = 0;
    if (account.data.analitpartner) {
        do_cpy(saldo_key0.p_g_code, key->p_g_code);
        saldo_key0.p_code = key->p_code;
    }
    else {
        do_text_set_empty(saldo_key0.p_g_code);
        saldo_key0.p_code = 0;
    }
    if (account.data.analitother)
        do_cpy(saldo_key0.other, key->other)
    else
        do_text_set_empty(saldo_key0.other);

    status = do_saldo_get0(alias, &saldo, &saldo_key0, DO_GET_EQUAL);
    if ( status == DO_ERROR )
        err = 1;

    if ( status == DO_KEY_NOT_FOUND ) {
        memset(&saldo.data, 0, sizeof(saldo.data));
        do_cpy(saldo.data.division, saldo_key0.division);
        do_cpy(saldo.data.layer, saldo_key0.layer);
        do_cpy(saldo.data.account, saldo_key0.account);
        do_cpy(saldo.data.account_total, saldo_key0.account_total);
        do_cpy(saldo.data.subaccount, saldo_key0.subaccount);
        do_cpy(saldo.data.subaccount_total, saldo_key0.subaccount_total);
        do_cpy(saldo.data.p_g_code, saldo_key0.p_g_code);
        do_cpy(saldo.data.p_code, saldo_key0.p_code);
        do_cpy(saldo.data.other, saldo_key0.other);
    }
    if ( !err ) {
        date_n = saldo.data.date_n;
        date_e = saldo.data.date;
        if (!date_n || date_n > key->date)
            date_n = key->date;
        if (!date_e || date_e < key->date)
            date_e = key->date;

        for(j = 0; j < 3; j ++) {
            if (sumincl[j]) {
                if ( accept ) {
                    sum_credit[j] = do_pdecimal(saldo.data.credit_sum[j], 8, 2) +
                                    key->credit_sum[j];
                    sum_debit[j] = do_pdecimal(saldo.data.debit_sum[j], 8, 2) +
                                    key->debit_sum[j];
                }
                else  {
                    sum_credit[j] = do_pdecimal(saldo.data.credit_sum[j], 8, 2) -
                                    key->credit_sum[j];
                    sum_debit[j] = do_pdecimal(saldo.data.debit_sum[j], 8, 2) -
                                    key->debit_sum[j];
                }
            }
        }

        for (j = 0; j < 3; j++) {
            if (sum_debit[j] < 0) {
                sum_credit[j] -= sum_debit[j];
                sum_debit[j] = 0;
            }
            if (sum_credit[j] < 0) {
                sum_debit[j] -= sum_credit[j];
                sum_credit[j] = 0;
            }
            if( (!account.data.analitsub && account.data.positivesaldo   ) ||
                ( account.data.analitsub && subaccount.data.positivesaldo) ) {
                if (sum_debit[j] > sum_credit[j]) {
                    sum_debit[j] -= sum_credit[j];
                    sum_credit[j] = 0;
                }
                else {
                    sum_credit[j] -= sum_debit[j];
                    sum_debit[j] = 0;
                }
            }
            if ( (!account.data.analitsub && (!account.data.debit || !account.data.credit)) ||
                 ( account.data.analitsub && (!subaccount.data.debit || !subaccount.data.credit)) ) {
                if ( (!account.data.analitsub && account.data.debit) ||
                     ( account.data.analitsub && subaccount.data.debit) ) {
                    sum_debit[j] -= sum_credit[j];
                    sum_credit[j] = 0;
                }
                else {
                    sum_credit[j] -= sum_debit[j];
                    sum_debit[j] = 0;
                }
            }
        }

        saldo.data.date_n = date_n;
        saldo.data.date = date_e;
        saldo.data.date_modify = 0;
        saldo.data.time_modify = 0;
        for (j = 0; j < 3; j++) {
            do_pdecimal_set(saldo.data.debit_sum[j], 8, 2, sum_debit[j]);
            do_pdecimal_set(saldo.data.credit_sum[j], 8, 2, sum_credit[j]);
        }
        if (status == DO_KEY_NOT_FOUND)
            err = do_saldo_insert(alias, &saldo) != DO_OK;
        else
            err = do_saldo_update(alias, &saldo) != DO_OK;
    }
    if (!err)
        err = do_alias_tran_end(alias) != DO_OK;
    else
        do_alias_tran_abort(alias);
    return !err;
}

