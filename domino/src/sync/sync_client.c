#include <getopt.h>
#include <signal.h>
#include <locale.h>

#include <domino.h>
#include "../misc/send_error.h"
#include "version.h"
#include "option_client.h"
#include "replic.h"
#define MAX_RECONNECT 1

const char *sync_client_name = "sync_client";

static option_client_t *opt = NULL;

static int init()
{
    //!!do_log_set_type(sync_client_name, opt->log_type, opt->log_file);
    if ( opt->pid_file ) {
      FILE *fp = fopen (opt->pid_file, "w");
      if (!fp)
	    fprintf(stderr, "Open error on pid file %s", opt->pid_file);
      else {
        unsigned int pid = getpid ();
        fprintf(fp, "%u\n", pid);
        if (fclose (fp))
    	    fprintf(stderr, "Close error on pid file %s", opt->pid_file);

      }
    }

    if  (!domino_init(opt->domino_config_file, FALSE, DO_PARAM_END))
        exit(1);
    return 1;
}
static int check_parcels(do_alias_t *alias, do_rpc_client_t *client, const char *sklad, do_extended_break_func break_func);
static int set_last_stock_replic(do_alias_t *alias, const char *sklad, struct tm *tm);

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "");
    do_rpc_client_t *client;
 	//do_data_t *in  = NULL;
 	do_data_t *out;
    obj_arg_t *obj;
    char *param, *buf;
    const char *command;
    int i, err = 0, len, indx;
    do_alias_t *alias;
    option_document_t *key;
    time_t now;
    struct tm tm;

    do_log_set_stdout();

    opt = option_client_new();
    char *sklad;
    if (option_client_parse_options(opt, argc, argv))
       exit(1);
    if (opt->show_version) {
        printf("sync_client " SYNC_CLIENT_RELEASE " (" ADATE ") \n");
        exit(0);
    }
    init();
    alias = domino_alias_new(opt->alias_name);
    if ( !alias || !do_alias_open(alias, 1) ) {
#ifdef SEND_ERROR_ONLY_WORK_TIME
        {
            if ( do_isworktime() )
                send_error_message("Ошибка открытия БД");
        }
#else
        send_error_message("Ошибка открытия БД");
#endif
        exit(1);
    }
    int base_parcel;
    char *d1, *d2;
    //char *video = NULL;
    char *errmodify = NULL;


    client = do_rpc_client_new(opt->host, opt->port, opt->user, opt->password, FALSE);
    err = !client
#ifdef GCRYPT
        || !do_rpc_client_crypt_init_key(client,(opt->key_file) ? opt->key_file : domino_key_file())
#endif
        ;
    len = 0;
    for (i = 0; i < MAX_RECONNECT && !err; i++) {
        if (do_rpc_client_connect(client)) {
            len = 1;
            break;
        }
        DO_SEC_SLEEP(1);
    }
    err = !len;
    if (!err) {
        out = do_data_new();
        for (indx = 0; !err && indx < opt->objs->count; indx++) {
            do_data_clear(out);
            obj = opt->objs->list[indx];
            switch (obj->obj) {
                case OBJ_DOCUMENT:
                    command = "get_document";
                    param = (char*) do_malloc(1024);
                    buf = (char*) do_malloc(1024);
                    len = 1024;
                    *param = '\0';
                    for (i = 0; i < obj->document.documents->count; i++) {
                        key = obj->document.documents->list[i];
                        if (strlen(param) + 10 + strlen(key->document) + strlen(key->dtype) + strlen(key->sklad) > len - 1) {
                            param = do_realloc(param, len*2);
                            len *=2;
                        }
                        sprintf(buf, "%s \"%s.%s.%s\"", param, key->dtype, key->sklad, key->document);
                        strcpy(param, buf);
                    }
                    do_free(buf);

                    do_log(LOG_INFO, "documents replication \"%s\"", param);
                    err = !do_rpc_client_send_command(client, command, param, NULL, out);
                    do_free(param);
                    if (!err)
                        err = !replic_document(alias, do_data_get(out), do_data_size(out), NULL);
                    break;
                case OBJ_DOCUMENT_PERIOD:
                    param = (char*) do_malloc(1024);
                    err = 0;
                    for (i = 0; i < obj->document.documents->count && !err; i++) {
                        key = obj->document.documents->list[i];
                        do_log(LOG_INFO, "documents replication \"%s\"", key->dtype);
                        command = "get_document_in_period";
                        if (obj->document.all)
                            sprintf(param, "\"%s.%s\" all", key->dtype, key->sklad);
                        else  {
                            d1 = do_datetoa(obj->document.date_beg);
                            d2 = do_datetoa(obj->document.date_end);
                                sprintf(param, "\"%s.%s\" %s %s", key->dtype, key->sklad, d1, d2);
                                do_free(d1);
                                do_free(d2);
                        }
                        do_log(LOG_INFO, "documents in period replication \"%s\"", param);
                        err = !do_rpc_client_send_command(client, command, param, NULL, out);
                        if (!err)
                            err = !replic_document_period(alias,
                                                    key->dtype, key->sklad,
                                                    &obj->document.date_beg, &obj->document.date_end,
                                                    obj->document.all,
                                                    do_data_get(out), do_data_size(out), NULL);
                    }
                    do_free(param);
                    break;
                case OBJ_DOCUMENT_MODIFIED:

                    command = "get_document_from_protocol";
                    param = (char*) do_malloc(1024);
                    buf = (char*) do_malloc(1024);
                    len = 1024;
                    *param = '\0';
                    for (i=0; i < obj->document.documents->count; i++) {
                        key = obj->document.documents->list[i];
                        if (strlen(param) + 10 + strlen(key->sklad) + strlen(key->dtype) > len - 1) {
                            param = do_realloc(param, len*2);
                            len *=2;
                        }
                        sprintf(buf, "%s \"%s.%s\"", param, key->dtype, key->sklad);
                        strcpy(param, buf);
                    }
                    d1 = do_datetoa(obj->document.date_beg);
                    d2 = do_datetoa(obj->document.date_end);
                    if (strlen(param) + 10 + strlen(d1) + strlen(d2) > len - 1) {
                        param = do_realloc(param, len*2);
                        len *=2;
                    }

                    sprintf(buf, "%s %s %s", param, d1, d2);
                    strcpy(param, buf);
                    do_free(buf);

                    do_free(d1);
                    do_free(d2);
                    do_log(LOG_INFO, "modified documents replication \"%s\"", param);
                    err = !do_rpc_client_send_command(client, command, param, NULL, out);
                    do_free(param);
                    if (!err)
                        err = !replic_document(alias, do_data_get(out), do_data_size(out), NULL);
                    break;
                case OBJ_CHECKS:
                    do_log(LOG_INFO, "checks replication");
                    command = "get_checks";
                    param = (char*) do_malloc(1024);
                    d1 = do_datetoa(obj->sklad_period.date_beg);
                    d2 = do_datetoa(obj->sklad_period.date_end);
                    sprintf(param, "\"%s\" %s %s", obj->sklad_period.sklad, d1, d2);
                    do_free(d1);
                    do_free(d2);
                    do_log(LOG_INFO,"%s %s", command, param);
                    err = !do_rpc_client_send_command(client, command, param, NULL, out);
                    do_free(param);
                    if (!err)
                        err = !replic_checks(alias,
                                                obj->sklad_period.sklad,
                                                &obj->sklad_period.date_beg, &obj->sklad_period.date_end,
                                                do_data_get(out), do_data_size(out), NULL);
                    break;
                case OBJ_PROTOCOL:
                    do_log(LOG_INFO, "protocol replication");
                    command = "get_protocol";
                    param = (char*) do_malloc(1024);
                    d1 = do_datetoa(obj->protocol.date_beg);
                    d2 = do_datetoa(obj->protocol.date_end);
                    sprintf(param, "%s %s", d1, d2);
                    do_free(d1);
                    do_free(d2);
                    do_log(LOG_INFO,"%s %s", command, param);
                    err = !do_rpc_client_send_command(client, command, param, NULL, out);
                    do_free(param);
                    if (!err)
                        err = !add_protocol(alias,
                                                &obj->protocol.date_beg, &obj->protocol.date_end,
                                                do_data_get(out), do_data_size(out), NULL);
                    break;
                case OBJ_PRODUCT:
                    do_log(LOG_INFO, "product replication");
                    command = "get_product";
                    param = (char*) do_malloc(strlen(obj->product.code) + 3);
                    sprintf(param, "\"%s\"", obj->product.code);
                    err = !do_rpc_client_send_command(client, command, param, NULL, out);
                    do_free(param);
                    if (!err)
                        err = !replic_product(alias, obj->product.code, do_data_get(out), do_data_size(out), NULL);
                    break;

                case OBJ_PRODUCTS:
                case OBJ_PARCELS:
                    do_log(LOG_INFO, "products replication");
                    command = "get_products";
                    param = (char*) do_malloc(200);
                    if (obj->obj == OBJ_PRODUCTS) {
                        strcpy(param, "base");
                        base_parcel = 0;
                        sklad = NULL;
                    }
                    else {
                        sprintf(param, "parcel \"%s\"", obj->parcels.sklad);
                        sklad = obj->parcels.sklad;
                        base_parcel = 1;
                    }

                    do_log(LOG_INFO, "get data %s",(obj->obj == OBJ_PRODUCTS) ? "product" : "parcel");
                    err = !do_rpc_client_send_command(client, command, param, NULL, out);
                    if (!err) {
                        do_log(LOG_INFO, "data received %s",(obj->obj == OBJ_PRODUCTS) ? "product" : "parcel");
                        err = !replic_products(alias, do_data_get(out), do_data_size(out), sklad, base_parcel, NULL);
                    }
                    do_data_clear(out);

                    command = "get_products_view";
                    if (!err) {
                        do_log(LOG_INFO, "get data view");
                        err = !do_rpc_client_send_command(client, command, param, NULL, out);
                    }
                    if (!err) {
                        do_log(LOG_INFO, "data received view");
                        err = !replic_products_view(alias, do_data_get(out), do_data_size(out), sklad, base_parcel, NULL);
                    }
                    do_data_clear(out);

                    command = "get_products_data";
                    if (!err) {
                        do_log(LOG_INFO, "get data product data");
                        err = !do_rpc_client_send_command(client, command, param, NULL, out);
                    }
                    if (!err) {
                        do_log(LOG_INFO, "data received product data");
                        err = !replic_products_data(alias, do_data_get(out), do_data_size(out), sklad, base_parcel, NULL);
                    }
                    do_data_clear(out);
                    command = "get_barcodes";
                    if (!err) {
                        do_log(LOG_INFO, "get data barcode");
                        err = !do_rpc_client_send_command(client, command, param, NULL, out);
                    }
                    if (!err) {
                        do_log(LOG_INFO, "data received barcode");
                        err = !replic_barcodes(alias, do_data_get(out), do_data_size(out), sklad, base_parcel, NULL);
                    }

                    break;
                case OBJ_PARTNERS:
                    do_log(LOG_INFO, "partners replication");
                    if (obj->partner.g_code[0] == '\0') {
                        command = "get_regions";
                        err = !do_rpc_client_send_command(client, command, "", NULL, out);
                        if (!err)
                            err = !replic_regions(alias, do_data_get(out), do_data_size(out), NULL);
                        param = do_strdup("");
                    }
                    else {
                        err = 0;
                        param = (char*) do_malloc(strlen(obj->partner.g_code) + 3);
                        sprintf(param, "\"%s\"", obj->partner.g_code);
                    }
                    command = "get_partners";
                    if (!err)
                        err = !do_rpc_client_send_command(client, command, param, NULL, out);
                    if (!err)
                        err = !replic_partners(alias, do_data_get(out), do_data_size(out), (*obj->partner.g_code !='\0') ?
                                            obj->partner.g_code : NULL, NULL);
                    command = "get_partners_view";
                    if (!err)
                        err = !do_rpc_client_send_command(client, command, param, NULL, out);
                    if (!err)
                        err = !replic_partners_view(alias, do_data_get(out), do_data_size(out), (*obj->partner.g_code !='\0') ?
                                            obj->partner.g_code : NULL, NULL);
                    command = "get_partners_data";
                    if (!err)
                        err = !do_rpc_client_send_command(client, command, param, NULL, out);
                    do_free(param);
                    if (!err)
                        err = !replic_partners_data(alias, do_data_get(out), do_data_size(out), (*obj->partner.g_code !='\0') ?
                                            obj->partner.g_code : NULL, NULL);

                    break;
                case OBJ_PARTNER:
                    do_log(LOG_INFO, "parnters replication");
                    command = "get_partner";
                    param = (char*) do_malloc(strlen(obj->partner.g_code) + 50);
                    sprintf(param, "\"%s.%d\"", obj->partner.g_code, obj->partner.code);
                    err = !do_rpc_client_send_command(client, command, param, NULL, out);
                    do_free(param);
                    if (!err)
                        err = !replic_partner(alias, obj->partner.g_code, obj->partner.code, do_data_get(out), do_data_size(out), NULL);
                    break;
                case OBJ_STOCKS1:
                    do_log(LOG_INFO, "stocks replication \"%s\"", obj->stocks.sklad);
                    command = "get_stocks";
                    param = (char*) do_malloc(strlen(obj->stocks.sklad) + 50);
                    sprintf(param, "\"%s\"", obj->stocks.sklad);
                    do_log(LOG_INFO,"read from remote"); //!!
                    now = time(NULL);
                    tm = *localtime(&now);
                    err = !do_rpc_client_send_command(client, command, param, NULL, out);
                    do_free(param);
                    do_log(LOG_INFO,"end read from remote"); //!!
                    if (!err)
                        err = !replic_stocks(alias, obj->stocks.sklad, do_data_get(out), do_data_size(out), NULL);
                    if ( !err )
                        set_last_stock_replic(alias, obj->stocks.sklad, &tm);
                    break;
                case OBJ_UPDATED_STOCKS:
                    do_log(LOG_INFO, "updated stock replication \"%s\"", obj->stocks.sklad);
                    param = (char*) do_malloc(strlen(obj->stocks.sklad) + 50);
                    sprintf(param, "%d \"%s\"", replic_get_last_index(alias), domino_unit());
                    command = "get_updated_stocks";
                    do_log(LOG_INFO,"read from remote"); //!!
                    err = !do_rpc_client_send_command(client, command, param, NULL, out);
                    do_free(param);
                    do_log(LOG_INFO,"end read from remote"); //!!
                    if ( !err )
                        err = !replic_updated_stocks(alias, do_data_get(out), do_data_size(out), NULL);
                    break;
                case OBJ_GROUPS:
                    do_log(LOG_INFO, "groups replication");
                    command = "get_class";
                    err = !do_rpc_client_send_command(client, command, "", NULL, out);
                    if (!err)
                        err = !replic_class(alias, do_data_get(out), do_data_size(out), NULL);
                    do_data_clear(out);

                    command = "get_groups";
                    if (!err)
                        err = !do_rpc_client_send_command(client, command, "", NULL, out);
                    if (!err)
                        err = !replic_groups(alias, do_data_get(out), do_data_size(out), NULL);
                    do_data_clear(out);

                    command = "get_subgroups";
                    if (!err)
                        err = !do_rpc_client_send_command(client, command, "", NULL, out);
                    if (!err)
                        err = !replic_subgroups(alias, do_data_get(out), do_data_size(out), NULL);

                    break;
                case OBJ_OPERATIONS:
                    do_log(LOG_INFO, "operations replication");
                    command = "get_operations";
                    err = !do_rpc_client_send_command(client, command, "", NULL, out);
                    if (!err)
                        err = !replic_operations(alias, do_data_get(out), do_data_size(out), NULL);
                    do_data_clear(out);
                    break;
                case OBJ_USERS:
                    do_log(LOG_INFO, "users replication");
                    command = "get_users";
                    err = !do_rpc_client_send_command(client, command, "", NULL, out);
                    if (!err)
                        err = !replic_users(alias, do_data_get(out), do_data_size(out), NULL);
                    do_data_clear(out);
                    break;
                case OBJ_SKLADS:
                    do_log(LOG_INFO, "sklads replication");
                    command = "get_sklads";
                    err = !do_rpc_client_send_command(client, command, "", NULL, out);
                    if (!err)
                        err = !replic_sklads(alias, do_data_get(out), do_data_size(out), NULL);
                    do_data_clear(out);
                    break;
                case OBJ_OTDELS:
                    do_log(LOG_INFO, "otdels replication");
                    command = "get_otdels";
                    err = !do_rpc_client_send_command(client, command, "", NULL, out);
                    if (!err)
                        err = !replic_otdels(alias, do_data_get(out), do_data_size(out), NULL);
                    do_data_clear(out);
                    break;
                case OBJ_REALIZATION: {
                    struct tm tm1;
                    do_log(LOG_INFO, "realization replication");
                    command = "get_realization";
                    param = (char*) do_malloc(1024);
                    tm1 = obj->sklad_period.date_beg;
                    tm1.tm_mday = 1;
                    d1 = do_datetoa(tm1);
                    d2 = do_datetoa(obj->sklad_period.date_end);
                    sprintf(param, "\"%s\" %s %s", obj->sklad_period.sklad, d1, d2);
                    do_free(d1);
                    do_free(d2);
                    do_log(LOG_INFO,"%s %s", command, param);
                    err = !do_rpc_client_send_command(client, command, param, NULL, out);
                    do_free(param);
                    if ( !err )
                        err = !replic_realization(alias,
                                                obj->sklad_period.sklad,
                                                &tm1, &obj->sklad_period.date_end,
                                                do_data_get(out), do_data_size(out), NULL);
                    break;
                    }
                case OBJ_MAKE_REALIZATION:
                    do_log(LOG_INFO, "make realization");
                    command = "make_realization";
                    param = (char*) do_malloc(1024);
                    d1 = do_datetoa(obj->sklad_period.date_beg);
                    d2 = do_datetoa(obj->sklad_period.date_end);
                    sprintf(param, "\"%s\" %s %s", obj->sklad_period.sklad, d1, d2);
                    do_free(d1);
                    do_free(d2);
                    do_log(LOG_INFO,"%s %s", command, param);
                    err = !do_rpc_client_send_command(client, command, param, NULL, out);
                    if ( err )
                        do_log(LOG_INFO, "Error");
                    else
                        do_log(LOG_INFO, "Ok");
                    do_free(param);
                    break;
                case OBJ_CHECK_PARCELS:
                    do_log(LOG_INFO, "check parcels \"%s\"", obj->stocks.sklad);
                    err = check_parcels(alias, client, obj->stocks.sklad, NULL);
                    break;
                case OBJ_CHECK_BALANCE_41: {
                    do_log(LOG_INFO, "check balance 41");
                    double balance, balance1;
                    int count, count1;
                    if ( !get_balance_41(alias, obj->check_balance_41.sklad, obj->check_balance_41.date_end, &balance, &count, NULL) ) {
                        do_log(LOG_INFO, "Error");
                        break;
                    }
                    command = "get_balance_41";
                    param = (char*) do_malloc(1024);
                    d1 = do_datetoa(obj->check_balance_41.date_end);
                    sprintf(param, "\"%s\" %s", obj->check_balance_41.sklad, d1);
                    do_free(d1);
                    do_log(LOG_INFO,"%s %s", command, param);
                    err = !do_rpc_client_send_command(client, command, param, NULL, out);
                    if ( err ) {
                        do_log(LOG_INFO, "Error");
                        break;
                    }
                    else
                        do_log(LOG_INFO, "Ok");
                    if ( do_data_size(out) != sizeof(balance1)+sizeof(count1) ) {
                        do_log(LOG_ERR, "Logical error");
                        err = TRUE;
                        break;
                    }
                    balance1 = *(double*)do_data_get(out);
                    count1 = *(int*)(do_data_get(out)+sizeof(double));
                    if ( balance < balance1 -0.001 || balance > balance1 + 0.001 || count != count1) {
                        d1 = do_datetoa(obj->check_balance_41.date_end);
                        errmodify = do_strdup_printf("Balance not equal %.2f %.2f %d %d from %s %s", balance,balance1,count,count1,obj->check_balance_41.sklad, d1);
                        do_log(LOG_ERR, errmodify);
                        do_free(d1);
                        err = 1;
                        break;
                    }
                    do_log(LOG_INFO, "equal");
                    break;
                }
            }
        }
        do_log(LOG_INFO, "replication error %d", err);
        do_data_free(out);
        do_rpc_client_disconnect(client);
    }
    else
        err = 1;

    if ( err ) {
        int i;
        char *errbuf;
        size_t buflen = 1024, len;
        errbuf = (char*)do_malloc(buflen);
        strcpy(errbuf, "Error from ");
        len = strlen(errbuf);
        if ( !gethostname(errbuf + len, buflen - len - 1) )
            len = strlen(errbuf);
        if ( errmodify ) {
            len = strlen(errbuf);
            errbuf = do_realloc(errbuf, len + strlen(errmodify) + 2);
            errbuf[len] = ' ';
            strcpy(errbuf + len +1, errmodify);
        }
        else
            for (i = 0; i < argc; i++) {
                if (buflen - len - 1 <= strlen(argv[i])) {
                    errbuf = do_realloc(errbuf, buflen * 2);
                    buflen *=2;
                }
                errbuf[len++] = ' ';
                strcpy(errbuf + len, argv[i]);
                len = strlen(errbuf);
            }
#ifdef SEND_ERROR_ONLY_WORK_TIME
        {
            if ( do_isworktime() )
                send_error_message(errbuf);
        }
#else
        send_error_message(errbuf);
#endif
        do_free(errbuf);
    }

    if (client)
        do_rpc_client_free(client);
    option_client_free(opt);
    do_alias_free(alias);
    return err;
}

#define RUNNING_ (!break_func || !break_func(NULL))
static int check_parcels(do_alias_t *alias, do_rpc_client_t *client, const char *sklad, do_extended_break_func break_func)
{
    do_ret_list_t *list;
    int i,err;
    stock_struct_t *stock_st;
    product_key0_t product_key0;
    int status;
    do_string_list_t *codes;

	do_log(LOG_INFO, "read from local");
    list = replic_select_stocks(alias, sklad, break_func);
	do_log(LOG_INFO, "end read from local");
	do_log(LOG_INFO, "check parcels");
	codes = do_string_list_new();

    err = 0;
    product_key0.base_parcel = 1;
    status = DO_OK;
    for ( i = 0; status != DO_ERROR && i < do_ret_list_count(list); i++ ) {
        if ( !RUNNING_ ) {
            status = DO_ERROR;
            break;
        }
		/*if ( !(i % 100) )
		   usleep(10000);*/
        stock_st = (stock_struct_t*)do_ret_list_item_data(list, i);
        if ( stock_st->code[5] == '.' ) {
            do_cpy(product_key0.code, stock_st->code);
            status = do_product_key0(alias, &product_key0, DO_GET_EQUAL);
            if ( status == DO_KEY_NOT_FOUND ) {
                do_log(LOG_INFO, "parcel %s not found", do_text(alias, product_key0.code));
                do_string_list_add(codes, do_text(alias, product_key0.code));
            }
        }
    }
	do_log(LOG_INFO, "end check parcels, %d error",codes->count);
    do_data_t *out;
    out = do_data_new();
	for ( i = 0; !err && i < codes->count; i++ ) {
        const char *command;
        char *param;

        do_data_clear(out);
        do_log(LOG_INFO, "product replication %s", codes->list[i]);
        command = "get_product";
        param = (char*) do_malloc(strlen(codes->list[i]) + 3);
        sprintf(param, "\"%s\"", (char*)codes->list[i]);
        err = !do_rpc_client_send_command(client, command, param, NULL, out);
        do_free(param);
        if ( !err )
            err = !replic_product(alias, codes->list[i], do_data_get(out), do_data_size(out), NULL);
	}
    return err;
}
static int set_last_stock_replic(do_alias_t *alias, const char *sklad, struct tm *tm)
{
    int status;
    enum_key0_t key;
    enum_key0_t enum_key0;
    enum_rec_t  enum_;
    char *name_;
    BTI_LONG date2;
    BTI_LONG date1;
    BTI_LONG time2;
    BTI_LONG time1;
    time_t now = time(NULL);
    struct tm tm1 = *localtime(&now);
    
    do_time_set(&time2, *tm);
    do_time_set(&time1, tm1);
    do_date_set(&date2, *tm);
    do_date_set(&date1, tm1);

    do_log(LOG_INFO, "set last stock replic %2.2d/%2.2d/%4.4d %2.2d:%2.2d:%2.2d", tm->tm_mday , tm->tm_mon + 1, tm->tm_year + 1900, tm->tm_hour, tm->tm_min, tm->tm_sec);//!!

    name_ = do_strdup_printf("SYNCSTOCK.%s", sklad);
    do_text_set(alias, enum_key0.name, name_);
    do_cpy(key.name, enum_key0.name);

    status = do_enum_get0(alias, &enum_, &enum_key0, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return FALSE;
    if ( status == DO_KEY_NOT_FOUND ||
         do_cmp(key.name, enum_key0.name) )
    {
        do_text_set(alias, enum_.data.name, name_);
        enum_.data.lastvalue = time1;
        enum_.data.step = date1;
        enum_.data.LowBound = date2;
        enum_.data.hiBound = time2;
        enum_.data.cycleFl = FALSE;
        do_text_set(alias, enum_.data.format, "N11");
        status = do_enum_insert(alias, &enum_);
    }
    else
    {
        enum_.data.step = date1;
        enum_.data.lastvalue = time1;
        enum_.data.hiBound = time2;
        enum_.data.LowBound = date2;
        status = do_enum_update(alias, &enum_);
    }
    return status == DO_OK;
}
