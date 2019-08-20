
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <time.h>
#include "dclub.h"
#include <domino.h>
#include "option.h"
#include <dolib.h>
#include "../misc/mproc_state.h"
#include "../misc/iconv_.h"
#include "../misc/csv_util.h"
#include "../misc/mask.h"
#ifdef _LINUX
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif


#define RUNNING (do_mproc_state_get_state() == DO_STATE_RUN && do_alias_conected(dclub->alias))
#define RUNNING_ ((!inproc || do_mproc_state_get_state() == DO_STATE_RUN) && do_alias_conected(alias))
#define WAIT_TIME_FOR_FILE 5
//#define DCLUB_CHARSET "CP866"
#define DCLUB_CHARSET UTF_CHARSET

#define REQUEST_SALE 1
#define REQUEST_UNDO 2
#define REQUEST_ACK 3

#define TYPE_ONE_POSITION 2



#ifdef _LINUX
static off_t file_size(const char* filename)
{
    struct stat buf;
    if (stat(filename, &buf) == -1) {
        do_log(LOG_ERR, "get stat file %s %s\n", filename, strerror(errno));
	return 0;
    }
    return buf.st_size;
}
#endif
void dclub_check_config(option_t *opt)
{
    DIR  *dirp = opendir (opt->path_input);
    if (!dirp)
        do_log(LOG_ERR, "Directory path-input FAIL \"%s\" ", opt->path_input);
    closedir(dirp);
    dirp = opendir (opt->path_output);
    if (!dirp)
        do_log(LOG_ERR, "Directory path-output FAIL \"%s\"", opt->path_output);
    closedir(dirp);
    dirp = opendir (opt->path_error);
    if (!dirp)
        do_log(LOG_ERR, "Directory path-error FAIL \"%s\"", opt->path_error);
    closedir(dirp);
}

static discount_t *insert_list(do_alias_t *alias, do_list_t *list, document_rec_t* document)
{
    int indx;
    for (indx = 0; indx < list->count; indx++) // sort by document desc
        if (strncmp(((discount_t*)list->list[indx])->key.document, document->data.document, sizeof(document->data.document)) < 0 )
            break;

    discount_t *discount = (discount_t*)do_malloc(sizeof(discount_t));
    memcpy(discount->key.dtype, document->data.dtype, sizeof(document->data.dtype));
    memcpy(discount->key.sklad, document->data.sklad, sizeof(document->data.sklad));
    memcpy(discount->key.document, document->data.document, sizeof(document->data.document));
    discount->scale = NULL;
    discount->perc = NULL;
    discount->description = NULL;

    discount->type_kind = do_document_param_int(alias, document, do_param(DO_PARAM_DOCUMENT_PARAMETR_DISCOUNT_KIND));
    discount->loss = do_document_param_int(alias, document, "LOSS");
    discount->one_off = do_document_param_int(alias, document, "OFF");
    discount->description = do_document_param(alias, document, do_param(DO_PARAM_DOCUMENT_PARAMETR_DISCOUNT_NAME));
    int type_calc = do_document_param_int(alias, document, do_param(DO_PARAM_DOCUMENT_PARAMETR_DISCOUNT_CALC));
    char *tail, *head, buff[100], *scale;
    int total, i, flag;
    switch (type_calc) {
        case 0:
            discount->scale = (double*) do_malloc(sizeof(double));
            discount->scale[0] = 0;
            discount->perc  = (double*) do_malloc(sizeof(double));
            discount->perc[0] = do_document_param_float(alias, document, do_param(DO_PARAM_DOCUMENT_PARAMETR_DISCOUNT_PERC));
            break;
        case 1:
            scale = do_document_param(alias, document, do_param(DO_PARAM_DOCUMENT_PARAMETR_DISCOUNT_SCALE));
            head = tail = scale;
            total = 0;
            for (; *head != '\0'; head++) if (*head == ',') total++;
            flag = 0;
            if (total > 1) {
                total = (total + 1) / 2;
                discount->scale = (double*) do_malloc(sizeof(double) * total);
                discount->perc  = (double*) do_malloc(sizeof(double) * total);
                flag = 1;
                for (i = 0; i < total; i++) {
                    head = tail;
                    while (*tail != ',' && *tail != '\0') tail++;
                    if (*tail == '\0') {
                        do_free(discount->scale);
                        do_free(discount->perc);
                        discount->scale = NULL;
                        discount->perc = NULL;
                        flag = 0;
                        break;
                    }
                    strncpy(buff, head, tail - head);
                    buff[tail - head] = '\0';
                    discount->scale[i] = do_atof(buff);
                    tail++;
                    head = tail;
                    while (*tail != ',' && *tail != '\0') tail++;
                    strncpy(buff, head, tail - head);
                    buff[tail - head] = '\0';
                    discount->perc[i] = do_atof(buff);
                    if (*tail != '\0') tail++;
                }
                flag = (flag) && (discount->scale[total - 1] == 0);
            }
            do_free(scale);
            if (flag) break;
        default:
            discount->scale = (double*) do_malloc(sizeof(double));
            discount->scale[0] = 0;
            discount->perc  = (double*) do_malloc(sizeof(double));
            discount->perc[0] = 0;
            char *dtype = do_text(alias, discount->key.dtype);
            char *document = do_text(alias, discount->key.document);
            char *sklad = do_text(alias, discount->key.sklad);
            sprintf(buff, "%s/%s.%s", dtype, document, sklad);
            do_free(dtype); do_free(document); do_free(sklad);
            do_log(LOG_ERR, "Error in document %s - scale param is bad", buff);
    }
    discount->limit_product = do_document_param_int(alias, document, do_param(DO_PARAM_DOCUMENT_PARAMETR_DISCOUNT_LIMIT_PRODUCT));
    discount->kind_limit = do_document_param_int(alias, document, "KIND_LIMIT");
    do_list_insert(list, indx, discount);
    return discount;
}

void trans_free(trans_t *trans)
{
    if (trans)	{
        int i;
        if ( trans->cardname )
            do_free(trans->cardname);
        for (i = 0; i < trans->rec_list->count; i++)
            rec_free(trans->rec_list->list[i]);
        do_list_free(trans->rec_list);
        do_free(trans);
    }
}
trans_t *trans_new()
{
    trans_t *trans = (trans_t *) do_malloc(sizeof(trans_t));
    if (!trans)
        return NULL;
    memset(trans, 0, sizeof(trans_t) );
    trans->rec_list = do_list_new(0);
    return trans;
}
int trans_load(dclub_t *dclub, const char* filename)
{
    FILE* fp;
    if ((fp = fopen(filename, "r")) == NULL )
    {
        do_log(LOG_ERR,"open file %s - %s", filename, strerror(errno));
        return 0;
    }
    size_t len = BUFSIZ;
    char* line = (char*) do_malloc(len + 1);
    char* convline = NULL;
    char* tail;
    int header = 1;
    int line_indx = 0;
    rec_t *crnt;
    while (!feof(fp))
    {
        char* cp;
        if ( fgets( line, len + 1, fp ) == NULL )
            break;
        while ((strlen(line) == len) && !feof( fp ))
        {
            line = (char*) do_realloc(line, len * 2 + 1);
            if (!fgets(line + len, len + 1, fp)) {
        	do_free(line);
	        do_log(LOG_ERR,"read file %s - %s", filename, strerror(errno));
        	return 0;
            }
            len *= 2;
        }
        line_indx++;
        if (convline) do_free(convline);

        convline = iconv_(dclub->fdclub_conv, line);
        cp = convline;
        CSV_DELSPACE(cp);

        tail = cp;
        while (!CSV_EOS(tail)) tail++;
        *tail = '\0';

        /* A leading '#' or ';' signals a comment line.  */
        if (cp[0] == '#' || cp[0] == ';' || cp[0] == '\0' || cp[0] == '\n')
            continue;

        if (header) // header part
        {
            if (!dclub->trans->request && csv_isname(&cp, "REQUEST"))
            {
                  if ( !strcmp(cp, "SALE" ) ) {
                      dclub->trans->request = REQUEST_SALE;
                      continue;
                  }
                  if ( !strcmp(cp, "SALE_UNDO") ) {
                      dclub->trans->request = REQUEST_UNDO;
                      continue;
                  }
                  if ( !strcmp(cp, "SALE_ACK") ) {
                      dclub->trans->request = REQUEST_ACK;
                      continue;
                  }

            }
            if (!dclub->trans->cash && csv_isname(&cp, "CASHCODE"))
            {
                  dclub->trans->cash = atoi(cp);
                  continue;
            }
            if (!dclub->trans->shift && csv_isname(&cp, "SHIFT"))
            {
                  dclub->trans->shift = atoi(cp);
                  continue;
            }
            if (!dclub->trans->scode && csv_isname(&cp, "SCODE"))
            {
                  dclub->trans->scode = atoi(cp);
                  continue;
            }
            if (!dclub->trans->time.tm_mday && csv_isname(&cp, "DATE"))
            {
                  do_atodate(cp, &dclub->trans->time);
                  continue;
            }
            //if (!dclub->trans->time.tm_hour && csv_isname(&cp, "TIME"))
            if (csv_isname(&cp, "TIME"))
            {
                  do_atotime(cp, &dclub->trans->time);
                  continue;
            }
            if (!dclub->trans->check && csv_isname(&cp, "CHECKNUM"))
            {
                  dclub->trans->check = atoi(cp);
                  continue;
            }
            if (!dclub->trans->cardno && csv_isname(&cp, "CARDNO"))
            {
                  dclub->trans->cardno = atoll(cp);
                  continue;
            }
/*            if (!*dclub->trans->card_type && csv_isname(&cp, "CARDTYPE"))
            {
                  strcpy(dclub->trans->card_type, cp);
                  cp = dclub->trans->card_type + strlen(dclub->trans->card_type);
                  while (cp[0] == '\0' || cp[0] == '\n' || cp[0] =='\r' || isspace ((unsigned char) cp[0]))
                    --cp;
                  cp[1] = '\0';
                  continue;
            }*/
            if (!dclub->trans->sum && csv_isname(&cp, "SUM"))
            {
                  dclub->trans->sum = do_atof(cp);
                  continue;
            }
            if (csv_isname(&cp, "T"))
            {
                header = 0;
            }
            else
                continue;
        }
        else
        {
            if (!csv_isname(&cp, "T"))
                continue;
        }
        // line
        crnt = rec_new();
        memset(crnt, 0, sizeof(rec_t) );
        crnt->position = atoi(csv_getelem(&cp));
        crnt->code = atoi(csv_getelem(&cp));

        crnt->barcode = do_strdup(csv_getelem(&cp));
        crnt->name = do_strdup(csv_getelem(&cp));
        crnt->bquant = do_atof(csv_getelem(&cp));
        crnt->price = do_atof(csv_getelem(&cp));
        crnt->price_i = do_atof(csv_getelem(&cp));
        crnt->sum_i = do_atof(csv_getelem(&cp));
        crnt->price_type = atoi(csv_getelem(&cp));
        crnt->price_mode = atoi(csv_getelem(&cp));
        crnt->cdept = do_strdup(csv_getelem(&cp));
        csv_getelem(&cp);
        //crnt->coef = atoi(csv_getelem(&cp));

        do_list_add(dclub->trans->rec_list, crnt);

    }
    if (convline)
        do_free(convline);
    do_free(line);
    fclose(fp);
    return 1;
}

int trans_upload(dclub_t *dclub, const char* filename,  char* message)
{
    FILE* fp;
    if ((fp = fopen(filename, "wb")) == NULL )
    {
        do_log(LOG_ERR,"create file %s - %s", filename, strerror(errno));
        return 0;
    }
#define LOG_AND_EXIT  {do_log(LOG_ERR, "write to file %s - %s", filename, strerror(errno)); fclose(fp); return 0;}
    if (!fprintf(fp, "REQUEST=SALE\r\n"))
        LOG_AND_EXIT;
    if (!fprintf(fp, "ID=\r\n"))
        LOG_AND_EXIT;
    if (!fprintf(fp, "CASHCODE=%d\r\n", dclub->trans->cash))
        LOG_AND_EXIT;
    if (!fprintf(fp, "SHIFT=%d\r\n", dclub->trans->shift))
        LOG_AND_EXIT;
    if (!fprintf(fp, "SCODE=%d\r\n", dclub->trans->scode))
        LOG_AND_EXIT;
    char *value = do_datetoa(dclub->trans->time);
    if (!fprintf(fp, "DATE=%s\r\n", value))
        LOG_AND_EXIT;
    do_free(value);
    value = do_timetoa(dclub->trans->time);
    if (!fprintf(fp, "TIME=%s\r\n", value))
        LOG_AND_EXIT;
    do_free(value);
    if (!fprintf(fp, "CHECKNUM=%d\r\n", dclub->trans->check))
        LOG_AND_EXIT;
    if (!fprintf(fp, "CARDNO=%ld\r\n", dclub->trans->cardno))
        LOG_AND_EXIT;
    if (!fprintf(fp, "CARDTYPE=A\r\n"))
        LOG_AND_EXIT;
    char buf[100];
    if (!fprintf(fp, "SUM=%s\r\n", do_ftoa(dclub->trans->sum, buf)))
        LOG_AND_EXIT;
    if (!fprintf(fp, "SUM1=%s\r\n",  do_ftoa(dclub->trans->sum, buf)))
        LOG_AND_EXIT;
    if (!fprintf(fp, "SUM2=%s\r\n",  do_ftoa(dclub->trans->sum, buf)))
        LOG_AND_EXIT;
    if (!fprintf(fp, "SUMTYPE=0\r\n"))
        LOG_AND_EXIT;
    if (!fprintf(fp, "ERRORCODE=%d\r\n", dclub->trans->error))
        LOG_AND_EXIT;
    if (!fprintf(fp, "DISCTYPE=T\r\n"))
        LOG_AND_EXIT;
    if (!fprintf(fp, "DISCABS=0\r\n"))
        LOG_AND_EXIT;
    if (!fprintf(fp, "DISCPERC=0\r\n"))
        LOG_AND_EXIT;
    if (!fprintf(fp, "DOCNUM=15\r\n"))
        LOG_AND_EXIT;
    char *mess = iconv_(dclub->msg_conv, message);
    if (!fprintf(fp, "MESSAGE=%s\r\n", mess))
        LOG_AND_EXIT;
    if (!fprintf(fp, "MESSAGE2=%s\r\n", mess))
        LOG_AND_EXIT;
    do_free(mess);
/*
#define LEN_MES 40

    char *head = messageoem, *tail = messageoem;
    char mess[LEN_MES + 1];
    while (*tail != '\0' && *tail != '\n' && *tail != '\r' && (tail - head) < LEN_MES) tail++;
    strncpy(mess, head, tail - head);
    mess[tail - head] = '\0';
    if (*tail == '\n') tail++;
    head = tail;
    if (!fprintf(fp, "MESSAGE=%s\r\n", mess))
        LOG_AND_EXIT;
    int i;
    for (i = 1; i <= 8 && *head != '\0'; i++) {
        while (*tail != '\0' && *tail != '\n' && *tail != '\r' && (tail - head) < LEN_MES) tail++;
        strncpy(mess, head, tail - head);
        mess[tail - head] = '\0';
        if (*tail == '\n' || *tail == '\r') tail++;
        head = tail;
        if (!fprintf(fp, "MESSAGE%d=%s\r\n", i, mess))
            LOG_AND_EXIT;
    };
    do_free(messageoem);
*/
    if (!fprintf(fp, "\r\n"))
        LOG_AND_EXIT;
    rec_t *crnt;
    int i;
    for (i = 0; i < dclub->trans->rec_list->count; i++) {
        crnt = dclub->trans->rec_list->list[i];
        if (!fprintf(fp, "T=%d,%d,%s,\"\",", crnt->position, crnt->code, crnt->barcode))
            LOG_AND_EXIT;

        if (!fprintf(fp, "%s,", do_ftoa(crnt->bquant, buf)))
            LOG_AND_EXIT;
        if (!fprintf(fp, "%s,", do_ftoa(crnt->price_i, buf)))
            LOG_AND_EXIT;
        if (!fprintf(fp, "%s,", do_ftoa(crnt->sum_i, buf)))
            LOG_AND_EXIT;
        if (!fprintf(fp, "%s,", do_ftoa(crnt->price_new, buf)))
            LOG_AND_EXIT;
        if (!fprintf(fp, "%s,%d,", do_ftoa(crnt->sum_new, buf), crnt->action))
            LOG_AND_EXIT;
        if (crnt->message)
            value = iconv_(dclub->msg_conv, crnt->message);
        else
            value = do_strdup("");
        if (!fprintf(fp, "\"%s\"\r\n", value))
            LOG_AND_EXIT;
        do_free(value);
    }
    fclose(fp);
    return 1;
}

//#define UNKNOWN "stranger"
void get_card_info(do_alias_t *alias, trans_t *trans, partner_rec_t *partner)
{
    struct tm tm;
    char *str;
    trans->cardname = do_text(alias, partner->data.name);
    trans->time_block = 0;
    trans->time_unblock = 0;

    str = do_partner_param(alias, partner, "ST_DATE");
    if ( do_atodate(str, &tm) )
        trans->time_block = mktime(&tm);
    do_free(str);
    str = do_partner_param(alias, partner, "RE_DATE");
    if ( do_atodate(str, &tm) )
        trans->time_unblock = mktime(&tm);
    do_free(str);
}
int discount_get_card_info(do_alias_t *alias, trans_t *trans)
{
    partner_rec_t partner;
    partner_key0_t partnerKey0;
    do_text_set(alias, partnerKey0.g_code, do_param(DO_PARAM_PARTNER_REGION_DISCOUNT));
    partnerKey0.code = trans->cardno;
    if ( do_partner_get0(alias, &partner, &partnerKey0, DO_GET_EQUAL) == DO_OK ) {
        get_card_info(alias, trans, &partner);
        return TRUE;
    }
    do_text_set(alias, partnerKey0.g_code, do_param(DO_PARAM_PARTNER_REGION_CASHIER));

    if ( trans->cardno < 2000 )
        partnerKey0.code = trans->cardno - 1000;
    else
    if ( trans->cardno < 20000 &&  trans->cardno > 10000 )
        partnerKey0.code = trans->cardno - 10000;

    if ( do_partner_get0(alias, &partner, &partnerKey0, DO_GET_EQUAL) == DO_OK ) {
        get_card_info(alias, trans, &partner);
        return TRUE;
    }

    else {
        trans->cardname = do_malloc(100);
        sprintf(trans->cardname, "%ld", trans->cardno);
        trans->time_block = 1;
        return FALSE;
    }
}
static int check_discount_limit(do_alias_t *alias, trans_t *trans, document_rec_t* document, const char* localunit,int inproc)
{
    int result = 1;
    int status;
    if (do_document_param_int(alias, document, do_param(DO_PARAM_DOCUMENT_PARAMETR_DISCOUNT_LIMIT_PRODUCT))) // check product limit
    {
        result = 0;
        int i;
        barcode_rec_t barcode;
        barcode_key0_t   barcodeKey0;
        document_order_key2_t    document_orderKey2;
        rec_t *crnt;
        for (i = 0; i < trans->rec_list->count; i++) {
            if (!RUNNING_) break;
            crnt = trans->rec_list->list[i];
            if ( !strncmp(crnt->barcode,"DC",2) )
                do_text_set(alias, barcodeKey0.barcode, crnt->barcode+2);
            else
                do_text_set(alias, barcodeKey0.barcode, crnt->barcode);
            status = do_barcode_get0(alias, &barcode, &barcodeKey0, DO_GET_EQUAL);
            if (status == DO_KEY_NOT_FOUND) {
                do_log(LOG_ERR, "product with barcode \"%s\" not fount", crnt->barcode);
                break;
            }
            if (status != DO_OK) break;
                memcpy(&document_orderKey2.dtype, &document->data.dtype, sizeof(document_orderKey2.dtype));
            memcpy(&document_orderKey2.sklad, &document->data.sklad, sizeof(document_orderKey2.sklad));
            memcpy(&document_orderKey2.document, &document->data.document, sizeof(document_orderKey2.document));
            barcode.data.code[do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH)] = '\0';
            do_text_set(alias, document_orderKey2.code, barcode.data.code);

            if ( do_document_order_key2(alias, &document_orderKey2, DO_GET_EQUAL) == DO_OK ) {
                result = 1;
                break;
            }
        }
    }
    if (!RUNNING_ || !result)  return 0;

    if (do_document_param_int(alias, document, do_param(DO_PARAM_DOCUMENT_PARAMETR_DISCOUNT_LIMIT_SUM))) // check sum limit
    {
        int days = do_document_param_int(alias, document, do_param(DO_PARAM_DOCUMENT_PARAMETR_DISCOUNT_LIMIT_PERIOD));
        if (days > 0)
        {
            /*!!time_t now = time(NULL);
            struct tm tm = *localtime(&now);
            tm.tm_mday -= days;
            now = mktime(&tm);
            double lower = do_document_param_float(alias, document, do_param(DO_PARAM_DOCUMENT_PARAMETR_DISCOUNT_LIMIT_SUM_LOWER));
            double upper = do_document_param_float(alias, document, do_param(DO_PARAM_DOCUMENT_PARAMETR_DISCOUNT_LIMIT_SUM_UPPER));
            double sum = 0;
            discount_rec_t discount;
            discount_key1_t discountKey1;
            discountKey1.card = trans->cardno;
            discountKey1.date = do_datetosql(tm);
            discountKey1.time = 0;
            status = do_discount_get1(alias, &discount, &discountKey1, DO_GET_GE);
            while (DO_OK == status) {
                if (!RUNNING_) break;
                if (discount.data.card != trans->cardno) break;
                sum += discount.data.sum;
                if (sum > upper) break;
                status = do_discount_get1(alias, &discount, &discountKey1, DO_GET_NEXT);
            }
            result = (sum >= lower) && (sum  < upper);
            */
            result = TRUE;
        }
    }
    if ( do_document_param_int(alias, document, "BLOCK") == 1) { // check block
        time_t now;
        now = time(NULL);
        result = ( !trans->time_block && !trans->time_unblock ) ||
                 ( ( trans->time_block && now < trans->time_block ) ||
                   ( trans->time_unblock && now >= trans->time_unblock ) );

    }
    if (!RUNNING_ || !result)  return 0;

    char *unit = do_document_param(alias, document, do_param(DO_PARAM_DOCUMENT_PARAMETR_DISCOUNT_LIMIT_SKLAD));
    result = !strlen(unit) || !strcmp(unit, localunit);
    do_free(unit);

    return result;
}
void discount_do_list_clear(do_list_t *list)
{
    int i;
    discount_t *discount;
    for (i = 0; i < list->count; i++) {
        discount = list->list[i];
        if (discount->description)
            do_free(discount->description);
        if (discount->scale)
            do_free(discount->scale);
        if (discount->perc)
            do_free(discount->perc);
    }
    do_list_clear(list);
}
/* backup from 01/08/2018
int discount_list(do_alias_t *alias, trans_t *trans, do_list_t *list, const char* unit, do_string_list_t *log, int inproc)
{
    document_key3_t documentKey3, baseKey, *buf;
    do_list_t *Keys = do_list_new(1);

    do_text_set(alias, baseKey.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_DISCOUNT));
    do_text_set(alias, baseKey.sklad, do_param(DO_PARAM_COMMON_SKLAD));
    memcpy(&documentKey3, &baseKey, sizeof(documentKey3.dtype) + sizeof(documentKey3.sklad));

    documentKey3.date = 0;
    documentKey3.time = 0;
    discount_do_list_clear(list);
    int status, i;
    status = do_document_key3(alias, &documentKey3, DO_GET_GT);
    while (DO_OK == status)
    {
        if (!RUNNING_) break;
        // check card range
        if (documentKey3.date > trans->cardno  ||
            memcmp(&documentKey3, &baseKey, sizeof(documentKey3.dtype) + sizeof(documentKey3.sklad))
	    )
            break;
        if (documentKey3.date <= trans->cardno && documentKey3.time >= trans->cardno) { // card is dia
            buf = (document_key3_t*) do_malloc(sizeof(document_key3_t));
            memcpy(buf, &documentKey3, sizeof(document_key3_t));
            do_list_add(Keys, buf);
        }
        status = do_document_key3(alias, &documentKey3, DO_GET_NEXT);
    }
    char *doc, *desc;
    if (RUNNING_ && Keys->count) {
        document_rec_t document;
        for (i =  Keys->count - 1; i >= 0; i-- ) {
            if (!RUNNING_) break;
            memcpy(&documentKey3, Keys->list[i], sizeof(document_key3_t));
            status = do_document_get3(alias, &document, &documentKey3, DO_GET_EQUAL);
            while (DO_OK == status) {
                if (!RUNNING_) break;

                if (check_discount_limit(alias, trans, &document, unit, inproc)) { // check for limit (sum, time, etc)
                    insert_list(alias, list, &document);
                    doc = do_text(alias, document.data.document);
                    desc = do_document_param(alias, &document, do_param(DO_PARAM_DOCUMENT_PARAMETR_DISCOUNT_NAME));
                    do_string_list_addf_alloc(log, "Number :%s; Range %d - %d; Description :%s;",
                                        doc, (int)document.data.date, (int)document.data.time, desc);
                    do_free(doc);
                    do_free(desc);

                }

                status = do_document_get3(alias, &document, &documentKey3, DO_GET_NEXT);
                if (memcmp(&documentKey3, &Keys[i], sizeof(documentKey3)))
                    break;
            }
        }
    }
    do_list_free(Keys);
    return list->count > 0;
}*/
int discount_list(do_alias_t *alias, trans_t *trans, do_list_t *list, const char* unit, do_string_list_t *log, int inproc)
{
    document_key0_t documentKey0, baseKey;
    document_rec_t document;

    document_data_key0_t document_data_key0;

    do_list_t *Keys = do_list_new(1);

    do_text_set(alias, baseKey.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_DISCOUNT));
    do_text_set(alias, baseKey.sklad, do_param(DO_PARAM_COMMON_SKLAD));
    do_text_set_empty(baseKey.document);
    do_cpy(documentKey0, baseKey);

    discount_do_list_clear(list);
    int status, i;
    status = do_document_get0(alias, &document, &documentKey0, DO_GET_GT);
    while (DO_OK == status)
    {
        if (!RUNNING_) break;
        if ( do_cmp(documentKey0.dtype, baseKey.dtype) || do_cmp(documentKey0.sklad, baseKey.sklad) ) break;

        // check card range
        if ( do_document_param_int(alias, &document, "CARDS") ) {
            do_cpy(document_data_key0.dtype, document.data.dtype);
            do_cpy(document_data_key0.sklad, document.data.sklad);
            do_cpy(document_data_key0.document, document.data.document);
            document_data_key0.type = 21;
            document_data_key0.number = trans->cardno;
            if ( do_document_data_key0(alias, &document_data_key0, DO_GET_EQUAL) == DO_OK )
                do_list_add_alloc(Keys, &document, sizeof(document));
        }
        else {
            if ( document.data.date <= trans->cardno && document.data.time >= trans->cardno ) // card is dia
                do_list_add_alloc(Keys, &document, sizeof(document));
        }
        status = do_document_get0(alias, &document, &documentKey0, DO_GET_NEXT);
    }
    char *doc, *desc;
    if (RUNNING_ && Keys->count) {
        document_rec_t *document;
        for (i =  Keys->count - 1; i >= 0; i-- ) {
            if (!RUNNING_) break;
            document = Keys->list[i];
            if (!RUNNING_) break;

            if ( check_discount_limit(alias, trans, document, unit, inproc) ) { // check for limit (sum, time, etc)
                insert_list(alias, list, document);
                doc = do_text(alias, document->data.document);
                desc = do_document_param(alias, document, do_param(DO_PARAM_DOCUMENT_PARAMETR_DISCOUNT_NAME));
                do_string_list_addf_alloc(log, "Number :%s; Range %d - %d; Description :%s;",
                                    doc, (int)document->data.date, (int)document->data.time, desc);
                do_free(doc);
                do_free(desc);


            }
        }
    }
    do_list_free(Keys);
    return list->count > 0;
}

static char *check_black_list(do_alias_t *alias, int cardno)
{
    partner_key0_t partner_key0;
    partner_rec_t partner;
    int status;
    do_text_set(alias, partner_key0.g_code, "SKB");
    partner_key0.code = cardno;
    status = do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL);
    if ( status == DO_KEY_NOT_FOUND )
        return NULL;
    if ( status == DO_ERROR )
        return do_strdup("Ошибка чтения БД");

    time_t now, d1,d2;
    struct tm tm;
    char *str;
    char *res = NULL;
    now = time(NULL);
    d1 = 0;
    d2 = 0;

    str = do_partner_param(alias, &partner, "ST_DATE");
    if ( do_atodate(str, &tm) )
        d1 = mktime(&tm);
    do_free(str);

    str = do_partner_param(alias, &partner, "RE_DATE");
    if ( do_atodate(str, &tm) )
        d2 = mktime(&tm);
    do_free(str);

//    do_log(LOG_INFO, "%d %d %d - %d",d1,now,d2,(!d1 || now >= d1) || (!d2 || now <= d2));
    if ( (!d1 || now >= d1) && (!d2 || now <= d2) ) {
       res = do_strdup_printf("Карта %d заблокирована %s %s", cardno, do_partner_param(alias, &partner, "ST_DATE"), do_partner_param(alias, &partner, "RE_DATE"));
    }
    return res;
}
static int discount_insert(dclub_t *dclub);
static int discount_undo(dclub_t *dclub);

static void dclub_trans(dclub_t *dclub, const char* filename, const char* filenameOut)
{
    char* fullname = (char*) do_malloc(strlen(dclub->opt->path_input) + strlen(filename) + 2);
    sprintf(fullname,"%s/%s", dclub->opt->path_input, filename);
    char* errname = (char*) do_malloc(strlen(dclub->opt->path_error) + strlen(filename) + 2);
    sprintf(errname,"%s/%s", dclub->opt->path_error, filename);
    remove(errname);

    int flagerr;

    for (flagerr = 0; (rename (fullname, errname) == -1) && (flagerr < WAIT_TIME_FOR_FILE); flagerr++)
	DO_SEC_SLEEP(1);
#ifdef _LINUX
    off_t size = -1;
    while (size != file_size(errname)) {
        size = file_size(errname);
        DO_SEC_SLEEP(1/5.);
    }
#endif

    if (flagerr == WAIT_TIME_FOR_FILE) {
        do_log(LOG_ERR, "rename file %s to %s %s\n", fullname, errname, strerror(errno));
        do_free(fullname);
        do_free(errname);
        return;
    }
    //do_log(LOG_INFO, "file %s to %s\n", fullname, errname);
    flagerr = 1;
    if (((dclub->trans = trans_new()) == NULL) || !trans_load(dclub, errname))
    {
        do_free(errname);
        trans_free(dclub->trans);
        dclub->trans = NULL;
        return;
    }
    if ( !dclub->trans->request || !dclub->trans->cash || !dclub->trans->shift || !dclub->trans->scode
       || !dclub->trans->time.tm_mday || !dclub->trans->check || !dclub->trans->cardno ||

       ( !dclub->trans->rec_list->count && dclub->trans->request == REQUEST_SALE) )  {
        if ( !dclub->trans->request )
            do_log(LOG_ALERT, "bad REQUEST in %s", filename);
        if ( !dclub->trans->cash )
            do_log(LOG_ALERT, "undefine CASH in %s", filename);
        if ( !dclub->trans->scode )
            do_log(LOG_ALERT, "undefine SCODE in %s", filename);
        if ( !dclub->trans->time.tm_mday )
            do_log(LOG_ALERT, "undefine DATE in %s", filename);
        /*if ( !dclub->trans->time.tm_hour )
            do_log(LOG_ALERT, "undefine TIME in %s", filename);*/
        if ( !dclub->trans->check )
            do_log(LOG_ALERT, "undefine CHECK in %s", filename);
        if ( !dclub->trans->cardno )
            do_log(LOG_ALERT, "undefine CARDNO in %s", filename);
        if ( !dclub->trans->shift )
            do_log(LOG_ALERT, "undefine SHIFT in %s", filename);
        if ( !dclub->trans->rec_list->count )
            do_log(LOG_ALERT, "not found product line in %s", filename);
    }
    else
    {
        char* fullnameOut = (char*) do_malloc(strlen(dclub->opt->path_output) + strlen(filenameOut) + 2);
        sprintf(fullnameOut,"%s/%s", dclub->opt->path_output, filenameOut);

        if ( (dclub->trans->cardno > 3999999999) && (dclub->opt->ext_program) ) {
            char *cmd;
            int err;
            cmd = do_strdup_printf("%s \"%s\" \"%s\"", dclub->opt->ext_program, errname, fullnameOut);
            do_log(LOG_INFO, "run %s", cmd);
            err = system(cmd);
            if ( err ) {
                do_log(LOG_ERR, "command %s return %d", cmd, err);
            }
            do_free(cmd);
        }
        else {

            do_string_list_clear(dclub->lasttrans);
            char message[1024];
            rec_t *crnt;
            int i;
            dclub->trans->error = 0;
            for (i = 0; i < dclub->trans->rec_list->count; i++) {
                crnt = dclub->trans->rec_list->list[i];
                crnt->action = 0;
                crnt->price_new = crnt->price_i;
                crnt->sum_new = crnt->sum_i;
                crnt->price_new_min = crnt->price_i;
                crnt->sum_new_min = crnt->sum_i;
            }
            discount_get_card_info(dclub->alias, dclub->trans);
            do_string_list_addf_alloc(dclub->lasttrans, "client %d %s", dclub->trans->cardno, dclub->trans->cardname);
            char *tmp_name, *block_str;
            tmp_name = iconv_(dclub->utf8_conv, dclub->trans->cardname);
            sprintf(message, "Клиент %s\n", tmp_name);
            do_free(tmp_name);
            if ( RUNNING ) {
                switch ( dclub->trans->request ) {
                    case REQUEST_SALE: {
                        block_str = check_black_list(dclub->alias, dclub->trans->cardno);
                        if ( block_str ) {
                            do_string_list_addf_alloc(dclub->lasttrans, "Card %d in black list", dclub->trans->cardno);
                            //sprintf(message, "Карта заблокирована %d\n", dclub->trans->cardno);
                            sprintf(message, "%s\n", block_str);
                            do_free(block_str);
                            dclub->trans->error = 1;
                            flagerr = 0;
                        }
                        else if ( !discount_list(dclub->alias, dclub->trans, dclub->discount_list, domino_unit(), dclub->lasttrans, 1) ) {
                            do_string_list_addf_alloc(dclub->lasttrans, "No discount for %s", dclub->trans->cardname);
                            do_log(LOG_ERR,"Нет скидок по карте %s", dclub->trans->cardname);
                            tmp_name = iconv_(dclub->utf8_conv, dclub->trans->cardname);
                            sprintf(message, "Нет скидок по карте %s\n", tmp_name);
                            //sprintf(message, "No discount for card %s\n", tmp_name);
                            do_free(tmp_name);
                            dclub->trans->error = 1;
                            flagerr = 0;
                        }
                        else {
                            char *error = NULL;
                            discount_t *discount;
                            for ( i = 0; i < dclub->discount_list->count; i++ ) {
                                discount = dclub->discount_list->list[i];
                                flagerr = !discount_calc(dclub->alias, dclub->trans, discount, domino_unit(), dclub->lasttrans, 1, &error, dclub->utf8_conv);
                                if ( flagerr ) {
                                    tmp_name = iconv_(dclub->utf8_conv, dclub->trans->cardname);
                                    if ( error ) {
                                        sprintf(message, "%s. Карта %s\n", error, tmp_name);
                                        do_free(error);
                                    }
                                    else
                                        sprintf(message, "Ошибка расчета скидки. Карта %s\n", tmp_name);
                                    do_free(tmp_name);
                                    dclub->trans->error = 1;
                                    flagerr = 0;
                                    break;
                                }
                            }
                            if ( !dclub->trans->error ) {
                                for ( i = 0; i < dclub->trans->rec_list->count; i++ ) {
                                    crnt = dclub->trans->rec_list->list[i];
                                    if ( crnt->price_new > crnt->price_new_min ) {
                                        crnt->price_new = crnt->price_new_min;
                                        crnt->message = do_strdup(crnt->message_min);
                                        crnt->action = 1;
                                    }
                                    if ( crnt->sum_new > crnt->sum_new_min ) {
                                        crnt->sum_new = crnt->sum_new_min;
                                        crnt->action = 1;
                                    }
                                }
                                flagerr = !discount_insert(dclub);
                            }
                        }
                        break;
                        }
                    case REQUEST_UNDO: {
                        flagerr = !discount_undo(dclub);
                        break;
                        }
                    default:
                        flagerr = FALSE;
                }
            }
            if (RUNNING && !flagerr) {
                trans_upload(dclub, fullnameOut, message);
#ifdef DEBUG
                char buff[1024];
                sprintf(buff, "%s.$$$$", fullnameOut);
                trans_upload(dclub, buff, message);
#endif
    //	    do_log(LOG_INFO, "%s\n", fullnameOut);
                do_free(fullnameOut);
                dclub->time_trans = time(NULL);
            }
#ifndef DEBUG
        if (!flagerr)
            if (remove(errname) == -1)
                do_log(LOG_ERR, "remove file %s %s\n", errname, strerror(errno));
#endif
        }
    }
    do_free(fullname);
    do_free(errname);
    trans_free(dclub->trans);
    dclub->trans = NULL;
}
typedef struct {
    char code[100];
    double quant;
} stock_t;

static int do_list_cmp_stock(stock_t *s1, stock_t *s2)
{
    return strcmp(s1->code,s2->code);
}

int discount_calc(do_alias_t *alias, trans_t *trans, discount_t *discount, const char* unit, do_string_list_t *log, int inproc, char **error, iconv_t utf8_conv)
{
    int i, k, flag_redef;
    int number_price_retail = 0;
    double quant, price_purchase, price_new, price_crnt_retail, price_i_retail;
    int status;
    barcode_rec_t barcode;
    barcode_key0_t   barcodeKey0;
    document_order_rec_t  document_order;
    document_order_key2_t    document_orderKey2, document_orderKey;
    product_rec_t product;
    product_key4_t   productKey4;
    sklad_rec_t  sklad;
    sklad_key0_t  skladKey0;
    rec_t *crnt;
    do_list_t *redef_list;
    //char *one_code=NULL;
    //double one_quant=0;
    double max_discount=0;
    double max_price=0;

    do_text_set(alias, &skladKey0.code, unit);
    status = do_sklad_get0(alias, &sklad, &skladKey0, DO_GET_EQUAL);
    redef_list = do_list_new(TRUE);
    if ( !RUNNING_ ) return 0;
    if ( (number_price_retail =
           do_sklad_param_int(alias, &sklad, do_param(DO_PARAM_SKLAD_PARAMETR_PRICE_RETAIL))) == 0)
        number_price_retail = do_param_int(DO_PARAM_PRICE_RETAIL_DEFAULT);
    char *code, *name;

    if ( discount->one_off == TYPE_ONE_POSITION ) { // only one product in discount card
        if ( !discount->limit_product ) {
            *error = do_strdup("Ошибка в правилах скидок");
            return FALSE;
        }
    }
    int retval;
    do_sort_list_t *max_quants;
    max_quants = do_sort_list_new(TRUE, TRUE, (do_list_cmp_func)do_list_cmp_stock, NULL);
    retval = TRUE;
    for ( i = 0; i < trans->rec_list->count && retval; i++ ) {

        if (!RUNNING_) return 0;
        crnt = trans->rec_list->list[i];
        crnt->action = 0;
        if ( !strncmp(crnt->barcode,"DC",2) )
            do_text_set(alias, barcodeKey0.barcode, crnt->barcode+2);
        else
            do_text_set(alias, barcodeKey0.barcode, crnt->barcode);
        if ( do_barcode_get0(alias, &barcode, &barcodeKey0, DO_GET_EQUAL) != DO_OK ) {
            do_log(LOG_ERR, "product with barcode \"%s\" not fount", crnt->barcode);
            continue;
        }

        memcpy(&productKey4.code, &barcode.data.code, sizeof(productKey4.code));
        if ( do_product_get4(alias, &product, &productKey4, DO_GET_EQUAL) != DO_OK ) {
            do_log(LOG_ERR, "product with barcode \"%s\" not fount", crnt->barcode);
            continue;
        }
        if ( do_product_param_int(alias, &product, "NO_DISCOUNT") )
           continue;

        do_cpy(crnt->product_key4.code, productKey4.code);

        crnt->coef = do_product_coef(alias, &product);

        price_crnt_retail = do_product_price(alias, &product, number_price_retail)*crnt->coef;
        //price_retail = crnt->price_i; //!!

        if ( discount->kind_limit ) {
            if ( do_product_param_int(alias, &product, do_param(DO_PARAM_PRODUCT_PARAMETR_CLASS)) == discount->kind_limit )
                continue;
        }


        if ( product.data.base_parcel )
            price_purchase = do_product_price(alias, &product, do_param_int(DO_PARAM_PRICE_PURCHASE))*crnt->coef;
        else {
            product_key0_t last_key;
            barcode_key1_t barcode_key1;
            product_rec_t last;
            last_key.base_parcel = 1;
            do_cpy(last_key.code,product.data.code);
            strncpy(last_key.code+5,".99999",6);
            price_purchase = 0;
            status = do_product_get0(alias, &last, &last_key, DO_GET_LT);
            while ( status == DO_OK ) {
                if ( !last.data.base_parcel ||
                     strncmp(last.data.code, product.data.code, 5) ) break;
                do_cpy(barcode_key1.code,last.data.code);
                if ( do_barcode_key1(alias, &barcode_key1, DO_GET_EQUAL) == DO_KEY_NOT_FOUND ) {
                    price_purchase = do_product_price(alias, &last, do_param_int(DO_PARAM_PRICE_PURCHASE))*crnt->coef;
                    break;
                }
                status = do_product_get0(alias, &last, &last_key, DO_GET_PREVIOUS);
            }
            if ( status == DO_ERROR ) {
                do_log(LOG_ERR, "product with barcode \"%s\" not found", crnt->barcode);
                continue;
            }
        }

    	if ( price_purchase <= 0 )
    	    price_purchase = price_crnt_retail;

        crnt->unpack = (crnt->coef > 1) && (price_crnt_retail > crnt->price*(crnt->coef - 0.5));
        price_i_retail = crnt->price_i * (crnt->unpack ? crnt->coef : 1);

        crnt->price_retail = price_i_retail/crnt->coef; // price_crnt_retail/coef;
        crnt->price_purchase = price_purchase/crnt->coef;


        quant = crnt->bquant * ((!crnt->unpack) ? crnt->coef : 1);
        crnt->quant_discount = quant;
        //!!crnt->price_after_discount = crnt->price_retail;


        name = do_text(alias, product.data.name);
        code = do_text(alias, product.data.code);
        do_string_list_addf_alloc(log, "Product %s %s price(purshase: %f,retail: %f)\n %s quant %f price %f",
                                  code, name, price_purchase, price_i_retail,
                                  (crnt->unpack)? "unpack":"", crnt->bquant, crnt->price_i);
        do_free(code);
        do_free(name);

        double quant_forsk, maxquant, perc;

        memcpy(&document_orderKey2, &discount->key, sizeof(document_key0_t));
        product.data.code[do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH)] = '\0';
        do_text_set(alias, document_orderKey2.code, product.data.code);
        memcpy(&document_orderKey, &document_orderKey2, sizeof(document_orderKey));
        price_new = price_i_retail;
        int redef = 0;
        status = do_document_order_get2(alias, &document_order, &document_orderKey2, DO_GET_EQUAL);
        if ( status == DO_OK ) { // redefinition discount for this product
            char *str;
            time_t d1,d2,now;
            struct tm tm;
            int no_unpack;

            quant_forsk = do_document_order_quant(alias, &document_order, 1);
            maxquant = do_document_order_quant(alias, &document_order, 2);
            no_unpack = do_document_order_param_int(alias, &document_order, "NO_UNPACK");
            max_discount = do_document_order_param_float(alias, &document_order, "MAX_DISCOUNT");
            max_price = do_document_order_param_float(alias, &document_order, "MAX_PRICE");


            //now = time(NULL);
            struct tm tm1;
            time ( &now );
            tm1 = *localtime(&now);
            tm1.tm_hour = 0;
            tm1.tm_min = 0;
            tm1.tm_sec = 0;
            now = mktime(&tm1);

            d1=0;d2=0;
            str = do_document_order_param(alias, &document_order, "DATE1");
            if ( do_atodate(str, &tm) )
                d1 = mktime(&tm);
            do_free(str);
            str = do_document_order_param(alias, &document_order, "DATE2");
            if ( do_atodate(str, &tm) )
                d2 = mktime(&tm);
            do_free(str);

            flag_redef = FALSE;
            if ( quant_forsk ) {
                flag_redef = !(quant_forsk == quant);
                if ( !flag_redef )  {
                    int indx;
                    for ( indx = 0; indx < redef_list->count; indx++ ) {
                        product_key4_t *key;
                        key = redef_list->list[indx];
                        if ( !do_cmp(key->code, document_order.data.code) ) {
                            flag_redef = TRUE;
                        }
                    }
                }
                if ( !flag_redef ) {
                    product_key4_t key;
                    do_cpy(key.code,document_order.data.code);
                    do_list_add_alloc(redef_list, &key, sizeof(key));
                }
            }
            if ( !flag_redef  &&
                 ((d1 == 0) || (d1 <= now)) &&
                 ((d2 == 0) || (d2 >= now)) &&
                 ((no_unpack == 0) || (crnt->unpack == 0))
                  )  {
                redef = 1;
            };
            if ( maxquant > 0 ) {
                stock_t rec, *l;
                int indx;
                strcpy(rec.code,code);
                rec.quant = 0;
                if ( !do_sort_list_find(max_quants, &indx, &rec) ) {
                    do_sort_list_insert_new(max_quants, indx, &rec, sizeof(rec));
                }
                l = max_quants->list[indx];
                l->quant += quant;
                if ( l->quant > maxquant ) {
                    char *name, *value;
                    value = do_text(alias, product.data.name);
                    name = iconv_(utf8_conv, value);
                    *error = do_strdup_printf("Превышено количество %.1f (макс %.1f) %s", l->quant/crnt->coef,maxquant/crnt->coef, name);
                    do_free(name);
                    do_free(value);
                    retval = FALSE;
                    break;
                }
            }
        };
        if ( !redef && discount->limit_product ) // string not fount
            continue;

        if ( redef ) { // redefinition discount for this product
            if (do_document_order_param_int(alias, &document_order, do_param(DO_PARAM_LINE_PARAMETR_DISCOUNT_REDEFINE))) { // string redefine discount
                switch (do_document_order_param_int(alias, &document_order, do_param(DO_PARAM_LINE_PARAMETR_DISCOUNT_KIND))) {
                    case 0:
                        price_new =  (price_i_retail * (100 - do_document_order_price(alias, &document_order, 2))/100);
                        do_string_list_addf_alloc(log, "redefine discount %s.%d discount %f percent new price %f", discount->description, document_order.data.line, do_document_order_price(alias, &document_order, 2), price_new);
                        break;
                    case 1:  // sum 1
                    default:
                        price_new =  (price_i_retail - do_document_order_price(alias, &document_order, 4));
                        do_string_list_addf_alloc(log, "redefine discount %s.%d discount %f new price %f", discount->description, document_order.data.line, do_document_order_price(alias, &document_order, 4), price_new);
                        break;

                }
                crnt->action = 1;
            }
        }
        if (crnt->action != 1) {
            switch (discount->type_kind) {
                case 0:
                    price_new = price_i_retail;
                    break;
                case 1:
                    price_new = price_purchase;
                    break;
                default:
                    price_new = price_i_retail;
            }
            perc = 0;
            for (k = 0; 1; k++)
                if (price_new < discount->scale[k] || discount->scale[k] == 0) {
                    perc = discount->perc[k];
                    break;
                }
            switch (discount->type_kind) {
                case 0:
                    price_new = price_i_retail * (100 - perc)/100;
                    do_string_list_addf_alloc(log, "discount %s discount %f new price %f", discount->description, perc, price_new);
                    break;
                case 1:
                    price_new = price_purchase *(100 + perc)/100;
                    do_string_list_addf_alloc(log, "discount %s markup %f new price %f", discount->description, perc, price_new);
                    break;
                default:
                    price_new = price_i_retail;
            }
        }
        //do_log(LOG_INFO, "redef %f, %f",price_new,price_retail);

        crnt->price_new = price_new;

        if ( !discount->loss && price_new < price_purchase )
            crnt->price_new = price_purchase;


        if ( crnt->price_new > price_i_retail )
            crnt->price_new = price_i_retail;

        if ( crnt->price_new < 0 )
            crnt->price_new = 0;//price_retail;

        if ( max_discount > 0 )
            if (  price_i_retail - crnt->price_new > max_discount)
                crnt->price_new = price_i_retail - max_discount;
        if ( max_price > 0 )
            if ( crnt->price_new > max_price )
                crnt->price_new = max_price;

        if ( crnt->unpack )
            crnt->price_new /= crnt->coef;

        crnt->price_new = do_real_to_money(crnt->price_new)/100.;
        if ( crnt->message )
            do_free(crnt->message);
        crnt->message = do_strdup(discount->description);
        if ( crnt->price_new < crnt->price_new_min ) {
            crnt->price_new_min = crnt->price_new;
            crnt->message_min = do_strdup(crnt->message);
        }

        crnt->sum_new = crnt->price_new*crnt->bquant;
        if ( crnt->sum_new < crnt->sum_new_min )
            crnt->sum_new_min = crnt->sum_new;
        crnt->action = 1;


        //!!crnt->price_after_discount = (unpack) ? crnt->price_new :  crnt->price_new / coef;

        if ( discount->one_off ) {
            /*if ( discount->one_off == 2 && trans->one_off == 2 ) {
                *error = do_strdup("Повторная позиция в чеке для однопозиционной карты");
                return FALSE;
            } */
            trans->one_off = discount->one_off;
        }
    }
    do_sort_list_free(max_quants);
    do_list_free(redef_list);
    if ( retval ) {
        double sumdiscount = 0, sum = 0;
        for (i = 0; i < trans->rec_list->count; i++) {
            crnt = trans->rec_list->list[i];
            if (crnt->action == 1) {
                sumdiscount += (crnt->sum_i - crnt->sum_new);
            }
            sum += crnt->sum_new;
        }
        do_string_list_addf_alloc(log, "discount card %d sum(%f, -%f)", trans->cardno, sum, sumdiscount);
    }
    return retval;
}
static int get_cash_code(do_alias_t *alias, const char *store, int number)
{
    partner_rec_t partner;
    partner_key0_t partner_key0,key;
    int status;
    char *value;

    do_text_set(alias, partner_key0.g_code, do_param(DO_PARAM_PARTNER_REGION_CASH));
    partner_key0.code = 0;
    do_cpy(key.g_code, partner_key0.g_code);

    status = do_partner_get0(alias, &partner, &partner_key0, DO_GET_GE);
    while ( status == DO_OK ) {
         if ( do_cmp(key.g_code, partner_key0.g_code) ) break;
         value = do_text(alias, partner.data.sklad);
         if ( !strcmp(value, store) ) {
             if ( do_partner_param_int(alias, &partner, "CASH") == number ) {
                 do_free(value);
                 return partner_key0.code;
             }
         }
         do_free(value);
         status = do_partner_get0(alias, &partner, &partner_key0, DO_GET_NEXT);
    }
    return number;
}
static int discount_insert(dclub_t *dclub)
{
    int i;
    rec_t *crnt;
    char buf[100];
    document_rec_t document;
    document_key0_t document_key0;
    int action;
    document_order_key0_t document_order_key0;
    document_order_rec_t document_order;
    do_alias_t *alias = dclub->alias;
    int status;
    double sum, discount;

    if ( dclub->trans->one_off ) {
        partner_key0_t partner_key0;
        partner_rec_t partner;

        do_text_set(alias, partner_key0.g_code, "SKB");
        partner_key0.code = dclub->trans->cardno;
        if ( do_partner_key0(alias, &partner_key0, DO_GET_EQUAL) != DO_KEY_NOT_FOUND )
            return FALSE;

        memset(&partner.data,0,sizeof(partner.data));
        //do_log(LOG_INFO, "block card %d", dclub->trans->cardno);
        do_text_set(alias, partner.data.g_code, "SKB");
        partner.data.code = dclub->trans->cardno;//+1000000000;
        partner.data.type = 7;
        do_text_set(alias, partner.data.name, "ЯЯЯ Astra одноразовая карта");
        do_partner_params_clear(alias, &partner);
        do_partner_param_set(alias, &partner, "MANUL", "1");

        if ( do_partner_insert(alias, &partner) != DO_OK )
            return FALSE;
        if ( !do_protocol_partner_add_rec(alias, &partner, DO_PROTOCOL_ACTION_INSERT) )
            return FALSE;
    }

    sprintf(buf, "%2.2d%4.4d%4.4d", dclub->trans->cash % 100, dclub->trans->shift, dclub->trans->check);
    do_text_set(alias, document_key0.dtype, "DK");
    do_text_set(alias, document_key0.sklad, domino_unit());
    do_text_set(alias, document_key0.document, buf);

    status = do_document_get0(alias, &document, &document_key0, DO_GET_EQUAL);
    if ( status == DO_ERROR )
        return FALSE;
    if ( status == DO_KEY_NOT_FOUND ) {
        do_cpy(document.data.dtype, document_key0.dtype);
        do_text_set(alias, document.data.dtype1, "1");
        do_text_set_empty(document.data.name);
        do_cpy(document.data.sklad, document_key0.sklad);
        do_cpy(document.data.document, document_key0.document);
        document.data.p_date=0;
        document.data.p_time=0;
        do_text_set_empty(document.data.party);
        document.data.accepted=0;
        do_document_params_clear(alias, &document);
        action = 1;
    }
    else {
        action = 0;
        do_cpy(document_order_key0.dtype, document_key0.dtype);
        do_cpy(document_order_key0.sklad, document_key0.sklad);
        do_cpy(document_order_key0.document, document_key0.document);
        document_order_key0.line=0;
        status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_GE);
        while ( status == DO_OK ) {
             if ( do_cmp(document_order_key0.dtype, document_key0.dtype) ||
                  do_cmp(document_order_key0.sklad, document_key0.sklad) ||
                  do_cmp(document_order_key0.document, document_key0.document) ) break;
             if ( do_document_order_delete(alias) != DO_OK )
                  return FALSE;

             status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_NEXT);
        }
        if ( status == DO_ERROR )
             return FALSE;

    }
    do_date_set(&document.data.date, dclub->trans->time);
    do_time_set(&document.data.time, dclub->trans->time);
    do_text_set(alias, document.data.p_g_code, do_param(DO_PARAM_PARTNER_REGION_CASH));
    document.data.p_code=get_cash_code(alias,domino_unit(),dclub->trans->cash);
    sprintf(buf, "%4.4d/%4.4d/%4.4d", dclub->trans->cash, dclub->trans->shift, dclub->trans->check);
    do_text_set(alias, document.data.p_doc, buf);
    do_document_param_int_set(alias, &document, "CARD", dclub->trans->cardno);

    sum = 0;
    discount = 0;
    for (i = 0; i < dclub->trans->rec_list->count; i++) {
        crnt = dclub->trans->rec_list->list[i];
        if (crnt->action == 1) {
            discount += (crnt->sum_i - crnt->sum_new);
        }
        sum += crnt->sum_new;
    }

    do_document_param_float_set(alias, &document, "CHECK_SUM", sum);
    do_document_param_float_set(alias, &document, "DISCOUNT_SUM", discount);


    if ( status == DO_KEY_NOT_FOUND )
        status = do_document_insert(alias, &document);
    else
        status = do_document_update(alias, &document);
    if ( status == DO_ERROR )
        return FALSE;


    do_cpy(document_order.data.dtype, document_key0.dtype);
    do_cpy(document_order.data.dtype1, document.data.dtype1);
    do_cpy(document_order.data.sklad, document_key0.sklad);
    do_cpy(document_order.data.document, document_key0.document);
    document_order.data.line=0;
    document_order.data.sort=0;
    //BTI_CHAR code[DOMINO_FIELD_CODE_SIZE];45
    document_order.data.date=document.data.date;
    document_order.data.time=document.data.time;
    document_order.data.accepted = 0;
    do_document_order_params_clear(alias, &document_order);
    do_document_order_quant_clear(alias, &document_order);
    do_document_order_price_clear(alias, &document_order);

    for (i = 0; i < dclub->trans->rec_list->count; i++) {
        crnt = dclub->trans->rec_list->list[i];
        document_order.data.line++;
        do_cpy(document_order.data.code, crnt->product_key4.code);
        do_document_order_quant_set(alias, &document_order, 1, crnt->quant_discount);
        do_document_order_price_set(alias, &document_order, 2, crnt->price_purchase);
        do_document_order_price_set(alias, &document_order, 4, crnt->price_retail);
        do_document_order_price_set(alias, &document_order, 9, (crnt->unpack) ? crnt->price_new : crnt->price_new/crnt->coef );
        do_document_order_param_set(alias, &document_order, "BAR", crnt->barcode);

        status = do_document_order_insert(alias, &document_order);

        if ( status == DO_ERROR )
            return FALSE;
    }

    status = do_protocol_document_add_rec(alias, &document, (action) ? DO_PROTOCOL_ACTION_INSERT : DO_PROTOCOL_ACTION_CHANGE);

    return status == DO_OK;
}
static int discount_undo(dclub_t *dclub)
{
    char buf[100];
    document_rec_t document;
    document_key0_t document_key0;
    document_order_key0_t document_order_key0;
    document_order_rec_t document_order;
    do_alias_t *alias = dclub->alias;
    int status;

    sprintf(buf, "%2.2d%4.4d%4.4d", dclub->trans->cash % 100, dclub->trans->shift, dclub->trans->check);
    do_text_set(alias, document_key0.dtype, "DK");
    do_text_set(alias, document_key0.sklad, domino_unit());
    do_text_set(alias, document_key0.document, buf);

    status = do_document_get0(alias, &document, &document_key0, DO_GET_EQUAL);
    if ( status == DO_ERROR )
        return FALSE;
    if ( status == DO_KEY_NOT_FOUND )
        return TRUE;
    if ( do_document_delete(alias) == DO_ERROR )
        return FALSE;

    do_cpy(document_order_key0.dtype, document_key0.dtype);
    do_cpy(document_order_key0.sklad, document_key0.sklad);
    do_cpy(document_order_key0.document, document_key0.document);
    document_order_key0.line=0;
    status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_GE);
    while ( status == DO_OK ) {
         if ( do_cmp(document_order_key0.dtype, document_key0.dtype) ||
              do_cmp(document_order_key0.sklad, document_key0.sklad) ||
              do_cmp(document_order_key0.document, document_key0.document) ) break;
         if ( do_document_order_delete(alias) != DO_OK )
              return FALSE;

         status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_NEXT);
    }
    if ( status != DO_ERROR ) {
        partner_key0_t partner_key0;
        partner_rec_t partner;

        do_text_set(alias, partner_key0.g_code, "SKB");
        partner_key0.code = dclub->trans->cardno;
        if ( do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL) == DO_OK ) {
            if ( do_partner_param_int(alias, &partner, "MANUL") == 1 ) {

                  status = do_partner_delete(alias);
            }
        }
    }

    if ( status == DO_ERROR )
         return FALSE;
    return TRUE;
}

dclub_t *dclub_new(option_t *opt, dclub_info_t *dclub_info, do_alias_t* alias)
{
    dclub_t *dclub = (dclub_t *) do_malloc(sizeof(dclub_t));
    dclub->opt = opt;
    dclub->lasttrans = do_string_list_new();
    dclub->discount_list = do_list_new(1);
    dclub->dclub_info = dclub_info;
    dclub->trans = NULL;
    dclub->alias = NULL;
    dclub->time_trans = 0;
    dclub->alias = alias;
    dclub->mask_in = mask_new(opt->mask_input,
#ifdef _LINUX
        opt->win_filename
#else
            1
#endif
        );
    dclub->mask_out = mask_new(opt->mask_output,
#ifdef _LINUX
        opt->win_filename
#else
            1
#endif
        );
#ifdef USE_UTF8
    char *str = UTF_CHARSET;
#else
    char *str = get_locale_lang();
#endif
    dclub->tdclub_conv = iconv_open(DCLUB_CHARSET, str);
    if (dclub->tdclub_conv == (iconv_t)-1)
        do_log(LOG_ERR, "Error iconv_open (\"%s\" to \"%s\") ", str, DCLUB_CHARSET);
    dclub->msg_conv = iconv_open(DCLUB_CHARSET, UTF_CHARSET);
    if (dclub->msg_conv == (iconv_t)-1)
        do_log(LOG_ERR, "Error iconv_open (\"%s\" to \"%s\") ", UTF_CHARSET, DCLUB_CHARSET);
    dclub->msg_conv_inv = iconv_open(UTF_CHARSET, DCLUB_CHARSET);
    if (dclub->msg_conv_inv == (iconv_t)-1)
        do_log(LOG_ERR, "Error iconv_open (\"%s\" to \"%s\") ", DCLUB_CHARSET, UTF_CHARSET);
    dclub->utf8_conv = iconv_open(UTF_CHARSET,str);
    if (dclub->utf8_conv == (iconv_t)-1)
        do_log(LOG_ERR, "Error iconv_open (\"%s\" to \"%s\") ", str, UTF_CHARSET);
    dclub->fdclub_conv = iconv_open(str, DCLUB_CHARSET);
    if (dclub->fdclub_conv == (iconv_t)-1)
        do_log(LOG_ERR, "Error iconv_open (\"%s\" to \"%s\") ", DCLUB_CHARSET, str);
    dclub_info_set(dclub_info, dclub->lasttrans, &dclub->time_trans);
    return dclub;
}

void dclub_free(dclub_t *dclub)
{
    discount_do_list_clear(dclub->discount_list);
    do_list_free(dclub->discount_list);
    do_string_list_free(dclub->lasttrans);
    mask_free(dclub->mask_in);
    mask_free(dclub->mask_out);
    if (dclub->fdclub_conv != (iconv_t)-1)
        iconv_close(dclub->fdclub_conv);
    if (dclub->tdclub_conv != (iconv_t)-1)
        iconv_close(dclub->tdclub_conv);
    if (dclub->msg_conv != (iconv_t)-1)
        iconv_close(dclub->msg_conv);
    if (dclub->msg_conv_inv != (iconv_t)-1)
        iconv_close(dclub->msg_conv_inv);
    if (dclub->utf8_conv != (iconv_t)-1)
        iconv_close(dclub->utf8_conv);
    do_free(dclub);
}

typedef struct {
    char filename[256];
    struct stat buf;
} fullfile_t;

static int do_list_cmp_file(fullfile_t *s1, fullfile_t *s2)
{
    return s1->buf.st_mtime-s2->buf.st_mtime;
}

void dclub_run(dclub_t *dclub)
{
    DIR  *dirp = opendir (dclub->opt->path_input);
    char *filename;
    do_sort_list_t *files;
    int i;
    if (!dirp) {
        do_log(LOG_ERR, "cannot open directory %s", dclub->opt->path_input);
        return;
    }
    struct dirent *filenext;
    files = do_sort_list_new(TRUE, FALSE, (do_list_cmp_func)do_list_cmp_file, NULL);
    while (RUNNING) {
        errno = 0;
        filenext = readdir(dirp);
        if (!filenext) {
            if (errno != 0)
                do_log(LOG_ERR, "reading directory (%s): %s", dclub->opt->path_input, strerror(errno));
            break;
        }

        if (mask_valid(dclub->mask_in, filenext->d_name)) {
            fullfile_t rec;
            strcpy(rec.filename,filenext->d_name);
            filename = do_strdup_path_join(NULL, dclub->opt->path_input, filenext->d_name, NULL);
            if (stat(filename, &rec.buf) == -1) {
                do_log(LOG_ERR, "get stat file %s %s\n", filenext->d_name, strerror(errno));
                return;
            }
            do_sort_list_add_alloc(files, &rec, sizeof(rec));
        }
    }
    closedir(dirp);

    for ( i = 0; i < files->count; i++ ) {
        fullfile_t *rec;
        rec = files->list[i];
        mask_valid(dclub->mask_in, rec->filename);
        filename = mask_out(dclub->mask_in, dclub->mask_out->format, dclub->mask_out->total);
        if (!filename) {
            do_log(LOG_ERR, "Output file mask less then input file mask");
            return;
        }
        if ( RUNNING )
            dclub_trans(dclub, rec->filename, filename);
        do_free (filename);
    }
    do_sort_list_free(files);

}
rec_t *rec_new()
{
    rec_t *rec = (rec_t*) do_malloc(sizeof(rec_t));
    memset(rec, 0, sizeof(rec_t));
    return rec;
}
void rec_free(rec_t *rec)
{
    if (rec->barcode)
        do_free(rec->barcode);
    if (rec->name)
        do_free(rec->name);
    if (rec->cdept)
        do_free(rec->cdept);
    if (rec->message)
        do_free(rec->message);
    if (rec->message_min)
        do_free(rec->message_min);
    do_free(rec);
}
