#include "forms.h"


#define MAX_STR 55
#define SEC_HEADER "Header"
#define SEC_FOOTER "Footer"
#define SEC_LINE "Line"
#define SEC_TABLE "Table"
#define SEC_TABLE_TOTAL "TableTotal"
#define SEC_TABLE_BREAK "TableBreak"
#define TEMPLATE_TORG12 "torg-12"
#define TEMPLATE_PROTOCOL "protocol"
#define TEMPLATE_SF "sf"
#define TEMPLATE_INVOICE "invoice"

static char *double_rest(double value)
{
    char buf[50];
    sprintf(buf, "%.3f", value);
    char *cp = buf + strlen(buf) - 1;
    while ( cp > buf && *cp == '0') cp--;
    if ( *cp == '.' ||
        *cp == ','
        ) cp--;
    cp[1] = '\0';
    return do_strdup(buf);
}
static char *double_money(double value)
{
    char buf[50];
    sprintf(buf, "%.2f", value);
    return do_strdup(buf);
}

#define VALUE_SET_INT(a,b) { value = do_malloc(50); \
			     sprintf(value, "%d", b); \
                             do_template_set(templ, a, value, NULL );\
                             do_free(value);}
#define VALUE_SET_REST(a,b) { value = double_rest(b); \
                             do_template_set(templ, a, value, NULL );\
                             do_free(value);}
#define VALUE_SET_MONEY(a,b) { value = double_money(b); \
                             do_template_set(templ, a, value, NULL );\
                             do_free(value);}
#define VALUE_SET_STR(a,b) { value = b; \
                             do_template_set(templ, a, value, NULL );\
                             do_free(value);}
#define DO_FREE(a) { if ( a ) do_free(a); }
do_template_t  *do_form_torg12(do_alias_t *alias, document_key0_t *document_key0)
{
    do_template_t *templ;
    document_rec_t document;
    document_order_rec_t document_order;
    document_order_key0_t document_order_key0;
    int line_number;
    char *value;
    //char buffer[1024];
    char *supplier_fullname = NULL;
    char *consignee_fullname = NULL;
    char *supplier_okpo = NULL;
    char *consignee_okpo = NULL;
    char *supplier_director_post = NULL;
    char *supplier_director_fio = NULL;
    char *supplier_book_fio = NULL;
    char *supplier_s_post = NULL;
    char *supplier_s_fio = NULL;
    char *consignee_post = NULL;
    char *consignee_fio = NULL;
    int in_, indx;

    templ = do_template_new(TEMPLATE_TORG12, do_alias_utf8(alias));
    if ( !templ )
        return NULL;

    do_template_cursor_t *header;
    do_template_cursor_t *footer;
    do_template_cursor_t *line;
    do_template_cursor_t *cursor;
    do_template_cursor_t *table;
    do_template_cursor_t *table_break;
//    do_template_cursor_t *table_total;

    cursor = do_template_cursor(templ);
    header = do_template_section(templ, SEC_HEADER);
    footer = do_template_section(templ, SEC_FOOTER);
    line = do_template_section(templ, SEC_LINE);
    table = do_template_section(templ, SEC_TABLE);
    table_break = do_template_section(templ, SEC_TABLE_BREAK);
    //table_total = do_template_section(templ, SEC_TABLE_TOTAL);

    switch ( do_document_get0(alias, &document, document_key0, DO_GET_EQUAL) ) {
        case DO_OK:
            break;
        case DO_KEY_NOT_FOUND:
            value = do_util_document_key0_to_str(alias, document_key0);
            do_log(LOG_ERR, "document \"%s\" not found", value);
            do_free(value);
        default:
            do_template_free(templ);
            return NULL;
    }

    value = do_text(alias, document_key0->dtype);
    in_ =!strcmp(value, do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_IN));
    do_free(value);

    partner_rec_t partner;
    partner_key0_t partner_key0;
    do_cpy(partner_key0.g_code, document.data.p_g_code);
    partner_key0.code = document.data.p_code;
    product_key4_t product_key4;
    product_rec_t product;

    switch ( do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL) ) {
        case DO_OK:
            break;
        case DO_KEY_NOT_FOUND:
            value = do_text(alias, partner_key0.g_code);
            do_log(LOG_ERR, "partner \"%s.%d\" not found", value, partner_key0.code);
            do_free(value);
        default:
            do_template_free(templ);
            return NULL;
    }
    if ( in_ ) {
        supplier_fullname = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_FULL_NAME));
        supplier_okpo = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_OKPO));
        supplier_director_post = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_DIRECTOR_POST));
        supplier_director_fio = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_DIRECTOR_FIO));
        supplier_book_fio = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_BOOK_FIO));
        supplier_s_fio = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_S_FIO));
        supplier_s_post = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_S_POST));
    }
    else {
        consignee_fullname = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_FULL_NAME));
        consignee_okpo = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_OKPO));
        consignee_fio = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_S_FIO));
        consignee_post = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_S_POST));
    }

    otdel_rec_t otdel;
    otdel_key0_t otdel_key0;
    struct tm time;

    do_text_set(alias, otdel_key0.code, domino_division());
    switch ( do_otdel_get0(alias, &otdel, &otdel_key0, DO_GET_EQUAL) ) {
        case DO_OK:
            break;
        case DO_KEY_NOT_FOUND:
            value = do_text(alias, otdel_key0.code);
            do_log(LOG_ERR, "otdel \"%s\" not found", value);
            do_free(value);
        default:
            do_template_free(templ);
            return NULL;
    }
    char *ch;
    value = do_otdel_param(alias, &otdel, do_param(DO_PARAM_OTDEL_PARAMETR_FIRM));
    if ( (ch = strchr(value, '.')) == NULL ) {
        do_free(value);
        do_log(LOG_ERR, "firm partner not set");
        do_template_free(templ);
        return NULL;
    }
    *ch = '\0';
    do_text_set(alias, partner_key0.g_code, value);
    partner_key0.code = atoi(ch + 1);
    do_free(value);
    switch ( do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL) ) {
        case DO_OK:
            break;
        case DO_KEY_NOT_FOUND:
            value = do_text(alias, partner_key0.g_code);
            do_log(LOG_ERR, "partner \"%s.%d\" not found", value, partner_key0.code);
            do_free(value);
        default:
            do_template_free(templ);
            return NULL;
    }
    if ( !in_ ) {
        supplier_fullname = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_FULL_NAME));
        supplier_okpo = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_OKPO));
        supplier_director_post = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_DIRECTOR_POST));
        supplier_director_fio = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_DIRECTOR));
        supplier_book_fio = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_BOOKKEEPER));
    }
    else {
        consignee_fullname = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_FULL_NAME));
        consignee_okpo = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_OKPO));
    }

    sklad_key0_t sklad_key0;
    sklad_rec_t sklad;
    int status;

    do_cpy(sklad_key0.code, document_key0->sklad);
    switch ( do_sklad_get0(alias, &sklad, &sklad_key0, DO_GET_EQUAL) ) {
        case DO_OK:
            break;
        case DO_KEY_NOT_FOUND:
            value = do_text(alias, sklad_key0.code);
            do_log(LOG_ERR, "sklad \"%s\" not found", value);
            do_free(value);
        default:
            do_template_free(templ);
            return NULL;
    }

    if ( in_ ) {
        consignee_fio = do_text(alias, sklad.data.mol);
        consignee_post = do_sklad_param(alias, &sklad, do_param(DO_PARAM_SKLAD_PARAMETR_POST));
    }
    else {
        supplier_s_fio = do_text(alias, sklad.data.mol);
        supplier_s_post = do_sklad_param(alias, &sklad, do_param(DO_PARAM_SKLAD_PARAMETR_POST));
    }

    value = do_text(alias, sklad.data.name);

    do_template_set(templ, "SupplierFullName", supplier_fullname,
                           "SupplierOKPO", supplier_okpo,
                           "ConsigneeFullName", consignee_fullname,
                           "ConsigneeOKPO", consignee_okpo, NULL);
    if ( !in_ )
        do_template_set(templ, "SkladName", value, NULL);
    do_free(value);
    value = do_text(alias, document.data.p_doc);
    do_template_set(templ, "DocumentNumber", value, NULL);
    do_free(value);
    do_date(document.data.date, &time);
    value = do_datetoa(time);
    do_template_set(templ, "DocumentDate", value, NULL);
    do_free(value);
    do_template_cursor_run_step_for(cursor, SEC_LINE);

    do_cpy(document_order_key0.dtype, document_key0->dtype);
    do_cpy(document_order_key0.sklad, document_key0->sklad);
    do_cpy(document_order_key0.document, document_key0->document);
    document_order_key0.line = 0;
    line_number = 0;
    indx = 0;
    int base_len;
    int nprice;
    if ( in_ )
        nprice = DO_CONST_PRICE_PURCHASE;
    else
        nprice = DO_CONST_PRICE_FACT;

    base_len = do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH);

    int nds_int;

    int64_t Total_ = 0, TOtal_ = 0;
    int64_t Sum_ = 0 , SUm_ = 0;
    int64_t NDSSum_ = 0, NDSSUm_ = 0;
    int64_t Quant_ = 0, QUant_ = 0;

    double total;
    //!!double sum;
    double ndssum;
    double quant;
    double nds_price;


    status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_GE);
    while ( status == DO_OK ) {
        char buf[1024];
        if ( do_cmp(document_order_key0.dtype, document_key0->dtype) ||
             do_cmp(document_order_key0.sklad, document_key0->sklad) ||
             do_cmp(document_order_key0.document, document_key0->document) ) break;

        line_number++;

        if ( line_number == MAX_STR + 1 ) {

            VALUE_SET_REST("Quant", Quant_/100.);
            VALUE_SET_MONEY("Sum", Sum_/100.);
            VALUE_SET_MONEY("NDSSum", NDSSum_/100.);
            VALUE_SET_MONEY("Total", Total_/100.);

            Total_ = 0;
            Sum_ = 0;
            NDSSum_ = 0;
            Quant_ = 0;

            do_template_cursor_first_run(footer);
            do_template_cursor_first_run(table_break);
            do_template_add_page_break(templ);
            do_template_cursor_first_run(table);
            do_template_cursor_first_run(header);
//            line_number = 1;
        }
        do_cpy(product_key4.code, document_order.data.code);
        switch ( do_product_get4(alias, &product, &product_key4, DO_GET_EQUAL) ) {
            case DO_OK:
                break;
            case DO_KEY_NOT_FOUND:
                value = do_text(alias, product_key4.code);
                do_log(LOG_ERR, "product \"%s\" not found", value);
                do_free(value);
            default:
                do_template_free(templ);
                return NULL;
        }
        indx++;
        sprintf(buf, "%d", indx);
        value = do_text(alias, product.data.name);
        do_template_set(templ, "Number", buf,
                               "ProductName", value,
                               NULL );
        do_free(value);
        strncpy(buf, product.data.code, base_len);
        buf[base_len] = '\0';
        value = do_text(alias, product.data.tech_unit);
        do_template_set(templ, "ProductCode", buf,
                               "EiName", do_unit_measure_name(14),
                               "EiCode", "796",
                               NULL );
        do_free(value);

        quant = do_document_order_quant(alias, &document_order, DO_CONST_QUANT_REST)/do_product_coef(alias, &product);
        Quant_ += do_real_to_money(quant);
        QUant_ += do_real_to_money(quant);
        VALUE_SET_REST("Quant", quant);
        if ( in_ )
            nds_int = do_document_order_param_int(alias, &document_order, do_param(DO_PARAM_DORDER_PARAMETR_NDS_SUPPLIER));
        else
            nds_int = do_document_order_param_int(alias, &document_order, do_param(DO_PARAM_DORDER_PARAMETR_NDS));

        if ( in_ )
            nds_price =  do_document_order_price(alias, &document_order, DO_CONST_PRICE_NDS_PURCHASE_);
        else  {
            nds_price =  (do_document_order_price(alias, &document_order, nprice) / (100. + nds_int))* nds_int;
        }

        VALUE_SET_MONEY("Price",
                             ( do_document_order_price(alias, &document_order, nprice) -
                               nds_price
                             ) * do_product_coef(alias, &product));
        /*sum = (do_document_order_price(alias, &document_order, nprice) -
               nds_price
               ) * do_document_order_quant(alias, &document_order, DO_CONST_QUANT_REST);*/

        if ( nds_int )
            sprintf (buf, "%d", nds_int);
        else
            sprintf (buf, "-");
        do_template_set(templ, "NDS", buf,
                               NULL );
        ndssum =  nds_price *
                   do_document_order_quant(alias, &document_order, DO_CONST_QUANT_REST);

        NDSSum_ += do_real_to_money(ndssum);
        NDSSUm_ += do_real_to_money(ndssum);
        if ( nds_int )
            VALUE_SET_MONEY("NDSSum", do_real_to_money(ndssum)/100.)
        else
            do_template_set(templ, "NDSSum", "", NULL);

        total =  do_document_order_price(alias, &document_order, nprice) *
                  do_document_order_quant(alias, &document_order, DO_CONST_QUANT_REST);

        Total_ += do_real_to_money(total);
        TOtal_ += do_real_to_money(total);
        VALUE_SET_MONEY("Total", do_real_to_money(total)/100.);
        Sum_ += do_real_to_money(total) - do_real_to_money(ndssum);
        SUm_ += do_real_to_money(total) - do_real_to_money(ndssum);
        VALUE_SET_MONEY("Sum", (do_real_to_money(total) - do_real_to_money(ndssum))/100.);

        do_template_cursor_first_run(line);

        status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_NEXT);
    }

    TOtal_ = do_real_to_money(do_document_sum(alias, &document, nprice));
    NDSSUm_ = do_real_to_money(do_document_sum(alias, &document, 16010)) +
             do_real_to_money(do_document_sum(alias, &document, 16020)) ;
    SUm_ = TOtal_ - NDSSUm_;
    VALUE_SET_REST("Quant", QUant_/100.);
    VALUE_SET_MONEY("Sum", SUm_/100.);
    VALUE_SET_MONEY("NDSSum", NDSSUm_/100.);
    VALUE_SET_MONEY("Total", TOtal_/100.);

    value = do_real_in_words(alias, indx, FALSE, FALSE);
    do_template_set(templ, "StrTotal", value, NULL);
    do_free(value);


    do_template_set(templ, "SourceDirectorPost", supplier_director_post,
                           "SourceDirectorFIO", supplier_director_fio,
                           "SourceBookFIO", supplier_book_fio,
                           "SourceSPost", supplier_s_post,
                           "SourceSFIO", supplier_s_fio,
                           "DestPost", consignee_post,
                           "DestFIO", consignee_fio,
                            NULL);

    value = do_real_in_words(alias, TOtal_/100., TRUE, TRUE);
    do_template_set(templ, "TotalInWords", value, NULL);
    do_free(value);

    do_template_cursor_skip(cursor);
    do_template_cursor_run(cursor);

    DO_FREE(supplier_fullname);
    DO_FREE(consignee_fullname);
    DO_FREE(supplier_okpo);
    DO_FREE(consignee_okpo);
    DO_FREE(supplier_director_post);
    DO_FREE(supplier_director_fio);
    DO_FREE(supplier_book_fio);
    DO_FREE(supplier_s_post);
    DO_FREE(supplier_s_fio);
    DO_FREE(consignee_post);
    DO_FREE(consignee_fio);

    if ( status == DO_ERROR ) {
        do_template_free(templ);
        return NULL;
    }
    return templ;
}

do_template_t  *do_form_sf(do_alias_t *alias, document_key0_t *document_key0)
{
    do_template_t *templ;
    document_rec_t document;
    document_order_rec_t document_order;
    document_order_key0_t document_order_key0;
    int line_number;
    char *value;
    char *saller_name = NULL;
    char *saller_address = NULL;
    char *saller_inn = NULL;
    char *client_name = NULL;
    char *client_address = NULL;
    char *client_inn = NULL;
    char *consignee_full_name = NULL;
    char *saller_director_fio = NULL;
    char *saller_book_fio = NULL;
    //char buffer[1024];

    int in_;

    templ = do_template_new(TEMPLATE_SF, do_alias_utf8(alias));
    if ( !templ )
        return NULL;

    do_template_cursor_t *line;
    do_template_cursor_t *cursor;

    cursor = do_template_cursor(templ);
    line = do_template_section(templ, SEC_LINE);

    switch ( do_document_get0(alias, &document, document_key0, DO_GET_EQUAL) ) {
        case DO_OK:
            break;
        case DO_KEY_NOT_FOUND:
            value = do_util_document_key0_to_str(alias, document_key0);
            do_log(LOG_ERR, "document \"%s\" not found", value);
            do_free(value);
        default:
            do_template_free(templ);
            return NULL;
    }
    struct tm time;

    value = do_text(alias, document_key0->dtype);
    in_ = !strcmp(value, do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_IN));
    do_free(value);
    value = do_document_param(alias, &document, do_param(DO_PARAM_DOCUMENT_PARAMETR_INVOICE));
    do_template_set(templ, "SFNumber", value, NULL);
    do_free(value);
    value = do_document_param(alias, &document, do_param(DO_PARAM_DOCUMENT_PARAMETR_INVOICE_DATE));
    do_atodate(value, &time);
    do_free(value);
    value = do_date_in_words(alias, &time);
    do_template_set(templ, "SFDate", value, NULL);
    do_free(value);


    partner_rec_t partner;
    partner_key0_t partner_key0;
    do_cpy(partner_key0.g_code, document.data.p_g_code);
    partner_key0.code = document.data.p_code;
    product_key4_t product_key4;
    product_rec_t product;

    switch ( do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL) ) {
        case DO_OK:
            break;
        case DO_KEY_NOT_FOUND:
            value = do_text(alias, partner_key0.g_code);
            do_log(LOG_ERR, "partner \"%s.%d\" not found", value, partner_key0.code);
            do_free(value);
        default:
            do_template_free(templ);
            return NULL;
    }
    if ( in_ ) {
        saller_name = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_NAME));
        saller_inn = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_INN_KPP));
        saller_address = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_ADDRESS));
        saller_director_fio = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_DIRECTOR_FIO));
        saller_book_fio = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_BOOK_FIO));
    }
    else {
        client_name = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_NAME));
        client_inn = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_INN_KPP));
        client_address = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_ADDRESS));
        consignee_full_name = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_FULL_NAME));
    }

    otdel_rec_t otdel;
    otdel_key0_t otdel_key0;

    do_text_set(alias, otdel_key0.code, domino_division());
    switch ( do_otdel_get0(alias, &otdel, &otdel_key0, DO_GET_EQUAL) ) {
        case DO_OK:
            break;
        case DO_KEY_NOT_FOUND:
            value = do_text(alias, otdel_key0.code);
            do_log(LOG_ERR, "otdel \"%s\" not found", value);
            do_free(value);
        default:
            do_template_free(templ);
            return NULL;
    }
    char *ch;
    value = do_otdel_param(alias, &otdel, do_param(DO_PARAM_OTDEL_PARAMETR_FIRM));
    if ( (ch = strchr(value, '.')) == NULL ) {
        do_free(value);
        do_log(LOG_ERR, "firm partner not set");
        do_template_free(templ);
        return NULL;
    }
    *ch = '\0';
    do_text_set(alias, partner_key0.g_code, value);
    partner_key0.code = atoi(ch + 1);
    do_free(value);
    switch ( do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL) ) {
        case DO_OK:
            break;
        case DO_KEY_NOT_FOUND:
            value = do_text(alias, partner_key0.g_code);
            do_log(LOG_ERR, "partner \"%s.%d\" not found", value, partner_key0.code);
            do_free(value);
        default:
            do_template_free(templ);
            return NULL;
    }
    if ( !in_ ) {
        saller_name = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_NAME));
        saller_inn = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_INN_KPP));
        saller_address = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_ADDRESS));
        saller_director_fio = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_DIRECTOR));
        saller_book_fio = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_BOOKKEEPER));
    }
    else {
        consignee_full_name = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_FULL_NAME));
        client_name = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_NAME));
        client_inn = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_INN_KPP));
        client_address = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_ADDRESS));
    }

    do_template_set(templ, "SellerName", saller_name,
                           "SellerAddress", saller_address,
                           "SellerINN", saller_inn,
                           "ConsigneeFullName", consignee_full_name,
                           "ClientName", client_name,
                           "ClientAddress", client_address,
                           "ClientINN", client_inn,
                           "DirectorFIO", saller_director_fio,
                           "BookFIO", saller_book_fio, NULL);


    value = do_text(alias, document.data.document);
    do_template_set(templ, "DocumentNumber", value, NULL);
    do_free(value);
    do_date(document.data.date, &time);
    value = do_datetoa(time);
    do_template_set(templ, "DocumentDate", value, NULL);
    do_free(value);
    do_template_cursor_run_step_for(cursor, SEC_LINE);

    do_cpy(document_order_key0.dtype, document_key0->dtype);
    do_cpy(document_order_key0.sklad, document_key0->sklad);
    do_cpy(document_order_key0.document, document_key0->document);
    document_order_key0.line = 0;
    line_number = 0;
    int base_len;
    int nprice;
    if ( in_ )
        nprice = DO_CONST_PRICE_PURCHASE;
    else
        nprice = DO_CONST_PRICE_FACT;

    base_len = do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH);


    int64_t Total_ = 0, TOtal_ = 0;
    int64_t Sum_ = 0 , SUm_ = 0;
    int64_t NDSSum_ = 0, NDSSUm_ = 0;
    int64_t Quant_ = 0, QUant_ = 0;
    int64_t NDSTotal[3];
    int64_t NDS[2];
    double total;
    //!!double sum;
    double ndssum;
    double nds_price;
    int    nds_int;
    double quant;
    int status;
    memset(NDSTotal, 0 ,sizeof(NDSTotal));
    memset(NDS, 0 ,sizeof(NDS));


    status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_GE);
    while ( status == DO_OK ) {
        char buf[1024];
        if ( do_cmp(document_order_key0.dtype, document_key0->dtype) ||
             do_cmp(document_order_key0.sklad, document_key0->sklad) ||
             do_cmp(document_order_key0.document, document_key0->document) ) break;

        line_number++;

        do_cpy(product_key4.code, document_order.data.code);
        switch ( do_product_get4(alias, &product, &product_key4, DO_GET_EQUAL) ) {
            case DO_OK:
                break;
            case DO_KEY_NOT_FOUND:
                value = do_text(alias, product_key4.code);
                do_log(LOG_ERR, "product \"%s\" not found", value);
                do_free(value);
            default:
                do_template_free(templ);
                return NULL;
        }
        value = do_text(alias, product.data.name);
        do_template_set(templ,
                               "ProductName", value,
                               NULL );
        do_free(value);
        strncpy(buf, product.data.code, base_len);
        buf[base_len] = '\0';
        value = do_text(alias, product.data.tech_unit);
        do_template_set(templ, "ProductCode", buf,
                               "ProductEI", do_unit_measure_name(14),
                               NULL );
        do_free(value);
        quant = do_document_order_quant(alias, &document_order, DO_CONST_QUANT_REST)/do_product_coef(alias, &product);
        Quant_ += do_real_to_money(quant);
        QUant_ += do_real_to_money(quant);
        VALUE_SET_REST("Quant", quant);

        if ( in_ )
            nds_int = do_document_order_param_int(alias, &document_order, do_param(DO_PARAM_DORDER_PARAMETR_NDS_SUPPLIER));
        else
            nds_int = do_document_order_param_int(alias, &document_order, do_param(DO_PARAM_DORDER_PARAMETR_NDS));

        if ( in_ )
            nds_price =  do_document_order_price(alias, &document_order, DO_CONST_PRICE_NDS_PURCHASE_);
        else  {
            nds_price =  (do_document_order_price(alias, &document_order, nprice) / (100. + nds_int))* nds_int;
        }

        VALUE_SET_MONEY("Price",
                             ( do_document_order_price(alias, &document_order, nprice) -
                               nds_price
                             ) * do_product_coef(alias, &product));
        /*sum = (do_document_order_price(alias, &document_order, nprice) -
               nds_price
               ) * do_document_order_quant(alias, &document_order, DO_CONST_QUANT_REST);*/
/*        Sum_ += do_real_to_money(sum);
        SUm_ += do_real_to_money(sum);
        VALUE_SET_MONEY("Sum", sum);*/

        if ( nds_int )
            sprintf (buf, "%d", nds_int);
        else
            sprintf (buf, "-");
        do_template_set(templ, "NDS", buf,
                               NULL );
        ndssum =  nds_price *
                   do_document_order_quant(alias, &document_order, DO_CONST_QUANT_REST);

        NDSSum_ += do_real_to_money(ndssum);
        NDSSUm_ += do_real_to_money(ndssum);
        if ( nds_int )
            VALUE_SET_MONEY("NDSSum", do_real_to_money(ndssum)/100.)
        else
            do_template_set(templ, "NDSSum", "", NULL);


        total =  do_document_order_price(alias, &document_order, nprice) *
                  do_document_order_quant(alias, &document_order, DO_CONST_QUANT_REST);

        Sum_ += do_real_to_money(total) - do_real_to_money(ndssum);
        SUm_ += do_real_to_money(total) - do_real_to_money(ndssum);
        VALUE_SET_MONEY("Sum", (do_real_to_money(total) - do_real_to_money(ndssum))/100.);

        switch ( nds_int ) {
    	    case 18:
    		NDSTotal[0] += do_real_to_money(total);
    		NDS[0] += do_real_to_money(ndssum);
    		break;
    	    case 10:
    		NDSTotal[1] += do_real_to_money(total);
    		NDS[1] += do_real_to_money(ndssum);
    		break;
    	    default:
    		NDSTotal[2] += do_real_to_money(total);
        }

        Total_ += do_real_to_money(total);
        TOtal_ += do_real_to_money(total);
        VALUE_SET_MONEY("Total", do_real_to_money(total)/100.);

        do_template_cursor_first_run(line);
        status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_NEXT);
    }
    TOtal_ = do_real_to_money(do_document_sum(alias, &document, nprice));
    NDSSUm_ = do_real_to_money(do_document_sum(alias, &document, 16010)) +
             do_real_to_money(do_document_sum(alias, &document, 16020)) ;

    NDS[0] = do_real_to_money(do_document_sum(alias, &document, 16020));
    NDS[1] = do_real_to_money(do_document_sum(alias, &document, 16010));
    NDSSUm_ = NDS[0]+NDS[1];
    SUm_ = TOtal_ - NDSSum_;

    NDSTotal[0] = do_real_to_money(do_document_sum(alias, &document, 9020));
    NDSTotal[1] = do_real_to_money(do_document_sum(alias, &document, 9010));
    NDSTotal[2] = do_real_to_money(do_document_sum(alias, &document, 9000));

    VALUE_SET_REST("Quant", QUant_/100.);
    VALUE_SET_MONEY("Sum", SUm_/100.);
    VALUE_SET_MONEY("NDSSum", NDSSUm_/100.);
    VALUE_SET_MONEY("Total", TOtal_/100.);
    VALUE_SET_MONEY("NDS18Total", NDSTotal[0]/100. - NDS[0]/100.);
    VALUE_SET_MONEY("NDS10Total", NDSTotal[1]/100. - NDS[1]/100.);
    VALUE_SET_MONEY("NDS00Total", NDSTotal[2]/100.);
    VALUE_SET_MONEY("NDS18", NDS[0]/100.);
    VALUE_SET_MONEY("NDS10", NDS[1]/100.);

    value = do_real_in_words(alias, TOtal_/100., TRUE, TRUE);
    do_template_set(templ, "TotalInWords", value, NULL);
    do_free(value);

    do_template_cursor_skip(cursor);
    do_template_cursor_run(cursor);

    DO_FREE(saller_name);
    DO_FREE(consignee_full_name);
    DO_FREE(saller_address);
    DO_FREE(saller_inn);
    DO_FREE(saller_director_fio);
    DO_FREE(saller_book_fio);
    DO_FREE(client_address);
    DO_FREE(client_inn);
    DO_FREE(client_name);

    if ( status == DO_ERROR ) {
        do_template_free(templ);
        return NULL;
    }
    return templ;
}

do_template_t  *do_form_protocol(do_alias_t *alias, document_key0_t *document_key0, int *found)
{
    do_template_t *templ;
    document_rec_t document;
    document_order_rec_t document_order;
    document_order_key0_t document_order_key0;
    char *value;
    char *supplier_name = NULL;
    char *client_name = NULL;
    char *sopr_doc;
    char *sopr_doc_date;
    //char buffer[1024];

    int in_, indx;

    templ = do_template_new(TEMPLATE_PROTOCOL, do_alias_utf8(alias));
    if ( !templ )
        return NULL;

    do_template_cursor_t *line;
    do_template_cursor_t *cursor;

    cursor = do_template_cursor(templ);
    line = do_template_section(templ, SEC_LINE);
    //table_total = do_template_section(templ, SEC_TABLE_TOTAL);

    switch ( do_document_get0(alias, &document, document_key0, DO_GET_EQUAL) ) {
        case DO_OK:
            break;
        case DO_KEY_NOT_FOUND:
            value = do_util_document_key0_to_str(alias, document_key0);
            do_log(LOG_ERR, "document \"%s\" not found", value);
            do_free(value);
        default:
            do_template_free(templ);
            return NULL;
    }
    struct tm time;

    value = do_text(alias, document_key0->dtype);
    in_ =!strcmp(value, do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_IN));
    do_free(value);

    sopr_doc = do_text(alias, document.data.p_doc);
    do_date(document.data.p_date, &time);
    sopr_doc_date = do_datetoa(time);

    partner_rec_t partner;
    partner_key0_t partner_key0;
    do_cpy(partner_key0.g_code, document.data.p_g_code);
    partner_key0.code = document.data.p_code;
    product_key4_t product_key4;
    product_rec_t product;

    *found = FALSE;
    switch ( do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL) ) {
        case DO_OK:
            break;
        case DO_KEY_NOT_FOUND:
            value = do_text(alias, partner_key0.g_code);
            do_log(LOG_ERR, "partner \"%s.%d\" not found", value, partner_key0.code);
            do_free(value);
        default:
            do_template_free(templ);
            return NULL;
    }
    if ( in_ ) {
        supplier_name = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_NAME));
    }
    else {
        client_name = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_NAME));
    }
    otdel_rec_t otdel;
    otdel_key0_t otdel_key0;

    do_text_set(alias, otdel_key0.code, domino_division());
    switch ( do_otdel_get0(alias, &otdel, &otdel_key0, DO_GET_EQUAL) ) {
        case DO_OK:
            break;
        case DO_KEY_NOT_FOUND:
            value = do_text(alias, otdel_key0.code);
            do_log(LOG_ERR, "otdel \"%s\" not found", value);
            do_free(value);
        default:
            do_template_free(templ);
            return NULL;
    }
    char *ch;
    value = do_otdel_param(alias, &otdel, do_param(DO_PARAM_OTDEL_PARAMETR_FIRM));
    if ( (ch = strchr(value, '.')) == NULL ) {
        do_free(value);
        do_log(LOG_ERR, "firm partner not set");
        do_template_free(templ);
        return NULL;
    }
    *ch = '\0';
    do_text_set(alias, partner_key0.g_code, value);
    partner_key0.code = atoi(ch + 1);
    do_free(value);
    switch ( do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL) ) {
        case DO_OK:
            break;
        case DO_KEY_NOT_FOUND:
            value = do_text(alias, partner_key0.g_code);
            do_log(LOG_ERR, "partner \"%s.%d\" not found", value, partner_key0.code);
            do_free(value);
        default:
            do_template_free(templ);
            return NULL;
    }
    if ( !in_ ) {
        supplier_name = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_NAME));
    }
    else {
        client_name = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_NAME));
    }

    do_template_set(templ, "SupplierName", supplier_name,
                           "ClientName", client_name,
                           "SoprNumber", sopr_doc,
                           "SoprDate", sopr_doc_date,
                           NULL);

    do_cpy(document_order_key0.dtype, document_key0->dtype);
    do_cpy(document_order_key0.sklad, document_key0->sklad);
    do_cpy(document_order_key0.document, document_key0->document);
    document_order_key0.line = 0;
    indx = 0;
    //int base_len;
   /*!! int nprice;
    if ( in_ )
        nprice = DO_CONST_PRICE_PURCHASE;
    else
        nprice = DO_CONST_PRICE_FACT;*/

    //base_len = do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH);
    int status;


    do_template_cursor_run_step_for(cursor, SEC_LINE);

    status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_GE);
    while ( status == DO_OK ) {
        char buf[1024];
        if ( do_cmp(document_order_key0.dtype, document_key0->dtype) ||
             do_cmp(document_order_key0.sklad, document_key0->sklad) ||
             do_cmp(document_order_key0.document, document_key0->document) ) break;

        do_cpy(product_key4.code, document_order.data.code);
        switch ( do_product_get4(alias, &product, &product_key4, DO_GET_EQUAL) ) {
            case DO_OK:
                break;
            case DO_KEY_NOT_FOUND:
                value = do_text(alias, product_key4.code);
                do_log(LOG_ERR, "product \"%s\" not found", value);
                do_free(value);
            default:
                do_template_free(templ);
                return NULL;
        }
        if ( do_product_param_int(alias, &product, do_param(DO_PARAM_PRODUCT_PARAMETR_CLASS)) ==
             DO_CONST_PRODUCT_CLASS_INDISPENSABLE_TO_LIFE ) {

            double price_fact_rz, pr, pr1;
            double price_rz, price40, price2;
            int valuta, nds_int;
            static char *valuta_name[]= {"RU", "USD", "EUR"};
            *found = TRUE;

            if ( in_ )
                nds_int = do_document_order_param_int(alias, &document_order, do_param(DO_PARAM_DORDER_PARAMETR_NDS_SUPPLIER));
            else
                nds_int = do_document_order_param_int(alias, &document_order, do_param(DO_PARAM_DORDER_PARAMETR_NDS));
            if ( in_ )
                price2 = do_document_order_price(alias, &document_order, DO_CONST_PRICE_PURCHASE)
                        *do_product_coef(alias, &product);
            else
                price2 = do_document_order_price(alias, &document_order, DO_CONST_PRICE_FACT)
                        *do_product_coef(alias, &product);

            price_fact_rz = do_document_order_param_float(alias, &document_order, do_param(DO_PARAM_DORDER_PARAMETR_FACT_RZ_PRICE));
            price_rz = do_document_order_param_float(alias, &document_order, "RZ_PRICE");
            valuta = do_document_order_param_int(alias, &document_order, "RZ_VALUTA");
            price40 = do_document_order_price(alias, &document_order, DO_CONST_PRICE_REGISTER)
                        *do_product_coef(alias, &product);
            pr = (price2/(100.+nds_int)*100 - price40/(100.+nds_int)*100);
            pr1 = pr/(price40/(100.+nds_int)*100)*100;

            indx++;
            sprintf(buf, "%d", indx);
            do_template_set(templ, "Number", buf, NULL);
            VALUE_SET_STR("ProductName", do_text(alias, product.data.name));
            VALUE_SET_STR("ProductSeria", do_product_param(alias, &product, do_param(DO_PARAM_PRODUCT_PARAMETR_SERIA)));
            VALUE_SET_STR("Manufactor", do_document_order_param(alias, &document_order, "RZ_MFR"));
            VALUE_SET_MONEY("PriceM-NDS", price_fact_rz);
            VALUE_SET_STR("ValutaAbr", do_strdup(valuta_name[valuta - 1]));
            VALUE_SET_MONEY("PriceM-Valuta", price_rz);
            VALUE_SET_MONEY("Price40-NDS", price40/(100.+nds_int)*100);
            VALUE_SET_MONEY("Price2-NDS", price2/(100.+nds_int)*100);
            VALUE_SET_MONEY("Price2", price2);
            VALUE_SET_MONEY("PriceM", price_fact_rz*(100.+nds_int)/100.);
            if ( pr > 0) {
                VALUE_SET_MONEY("ProfitP", pr1);
                VALUE_SET_MONEY("ProfitSum", pr);
            }
            else  {
                do_template_set(templ, "ProfitP","-",
                                       "ProfitSum","-",
                                       NULL);
            }


            do_template_cursor_first_run(line);
        }

        status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_NEXT);
    }

    do_template_cursor_skip(cursor);
    do_template_cursor_run(cursor);

    DO_FREE(supplier_name);
    DO_FREE(client_name);
    DO_FREE(sopr_doc);
    DO_FREE(sopr_doc_date);

    if ( status == DO_ERROR ) {
        do_template_free(templ);
        return NULL;
    }
    return templ;
}
do_template_t *do_form_invoice(do_alias_t *alias, document_key0_t *document_key0, int number)
{
    do_template_t *templ;
    document_rec_t document;
    document_order_rec_t document_order;
    document_order_key0_t document_order_key0;
    char *value;
    char *supplier_name = NULL;
    char *supplier_address = NULL;
    char *client_name = NULL;
    char *document_name;
    char *document_date;
    char *sopr_doc;
    char buffer[1024];
    char *mfr;

    int indx;

    templ = do_template_new(TEMPLATE_INVOICE, do_alias_utf8(alias));
    if ( !templ )
        return NULL;

    do_template_cursor_t *line;
    do_template_cursor_t *cursor;

    cursor = do_template_cursor(templ);
    line = do_template_section(templ, SEC_LINE);

    switch ( do_document_get0(alias, &document, document_key0, DO_GET_EQUAL) ) {
        case DO_OK:
            break;
        case DO_KEY_NOT_FOUND:
            value = do_util_document_key0_to_str(alias, document_key0);
            do_log(LOG_ERR, "document \"%s\" not found", value);
            do_free(value);
        default:
            do_template_free(templ);
            return NULL;
    }
    struct tm time;

    sopr_doc = do_text(alias, document.data.p_doc);
    do_date(document.data.p_date, &time);
    document_date = do_datetoa(time);

    partner_rec_t partner;
    partner_key0_t partner_key0;
    do_cpy(partner_key0.g_code, document.data.p_g_code);
    partner_key0.code = document.data.p_code;
    product_key4_t product_key4;
    product_rec_t product;

    switch ( do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL) ) {
        case DO_OK:
            break;
        case DO_KEY_NOT_FOUND:
            value = do_text(alias, partner_key0.g_code);
            do_log(LOG_ERR, "partner \"%s.%d\" not found", value, partner_key0.code);
            do_free(value);
        default:
            do_template_free(templ);
            return NULL;
    }
    client_name = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_NAME));

    otdel_rec_t otdel;
    otdel_key0_t otdel_key0;

    do_text_set(alias, otdel_key0.code, domino_division());
    switch ( do_otdel_get0(alias, &otdel, &otdel_key0, DO_GET_EQUAL) ) {
        case DO_OK:
            break;
        case DO_KEY_NOT_FOUND:
            value = do_text(alias, otdel_key0.code);
            do_log(LOG_ERR, "otdel \"%s\" not found", value);
            do_free(value);
        default:
            do_template_free(templ);
            return NULL;
    }
    char *ch;
    value = do_otdel_param(alias, &otdel, do_param(DO_PARAM_OTDEL_PARAMETR_FIRM));
    if ( (ch = strchr(value, '.')) == NULL ) {
        do_free(value);
        do_log(LOG_ERR, "firm partner not set");
        do_template_free(templ);
        return NULL;
    }
    *ch = '\0';
    do_text_set(alias, partner_key0.g_code, value);
    partner_key0.code = atoi(ch + 1);
    do_free(value);
    switch ( do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL) ) {
        case DO_OK:
            break;
        case DO_KEY_NOT_FOUND:
            value = do_text(alias, partner_key0.g_code);
            do_log(LOG_ERR, "partner \"%s.%d\" not found", value, partner_key0.code);
            do_free(value);
        default:
            do_template_free(templ);
            return NULL;
    }
    supplier_name = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_NAME));
    supplier_address = do_partner_param(alias, &partner, do_param(DO_PARAM_PARTNER_PARAMETR_FIRM_ADDRESS));
    document_name = do_malloc(100);
    sprintf(document_name, "Пр-%7.7d", number);

    do_template_set(templ, "SupplierName", supplier_name,
			   "SupplierAddress", supplier_address,
			   "DocumentName", document_name,
			   "DocumentDate", document_date,
			   "SourceDate", document_date,
                           "ClientName", client_name,
                           "SourceDocument", sopr_doc,
                           NULL);

    do_cpy(document_order_key0.dtype, document_key0->dtype);
    do_cpy(document_order_key0.sklad, document_key0->sklad);
    do_cpy(document_order_key0.document, document_key0->document);
    document_order_key0.line = 0;
    indx = 0;
    //int base_len;
    //int nprice;
    //'nprice = DO_CONST_PRICE_FACT;

    //base_len = do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH);
    int status;

    do_template_cursor_run_step_for(cursor, SEC_LINE);

    status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_GE);
    while ( status == DO_OK ) {
        //char buf[1024];
        if ( do_cmp(document_order_key0.dtype, document_key0->dtype) ||
             do_cmp(document_order_key0.sklad, document_key0->sklad) ||
             do_cmp(document_order_key0.document, document_key0->document) ) break;

        do_cpy(product_key4.code, document_order.data.code);
        switch ( do_product_get4(alias, &product, &product_key4, DO_GET_EQUAL) ) {
            case DO_OK:
                break;
            case DO_KEY_NOT_FOUND:
                value = do_text(alias, product_key4.code);
                do_log(LOG_ERR, "product \"%s\" not found", value);
                do_free(value);
            default:
                do_template_free(templ);
                return NULL;
        }
        mfr = do_document_order_param(alias, &document_order, "RZ_MFR");
        if ( *mfr == '\0' ) {
	    do_cpy(partner_key0.g_code, product.data.mfr_region);
	    partner_key0.code = product.data.mfr_code;
	    switch ( do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL) ) {
    		case DO_OK:
        	    break;
    		case DO_KEY_NOT_FOUND:
        	    value = do_text(alias, partner_key0.g_code);
        	    do_log(LOG_ERR, "partner \"%s.%d\" not found", value, partner_key0.code);
        	    do_free(value);
    		default:
        	    do_template_free(templ);
        	    return NULL;
	    }
	    mfr = do_text(alias, partner.data.name);
        }
        indx++;
	VALUE_SET_INT("Number", indx);
        VALUE_SET_STR("Name", do_text(alias, product.data.name));
        VALUE_SET_STR("Manufactor", mfr);
        VALUE_SET_STR("Seria", do_product_param(alias, &product, do_param(DO_PARAM_PRODUCT_PARAMETR_SERIA)));
        if ( do_document_order_price(alias, &document_order, 40) > 0 )
            VALUE_SET_MONEY("PriceMfr", do_document_order_price(alias, &document_order, 40)*do_product_coef(alias, &product))
        else
            VALUE_SET_STR("PriceMfr", do_strdup("-"));
        VALUE_SET_MONEY("Price", do_document_order_price(alias, &document_order, 9)*do_product_coef(alias, &product));
        VALUE_SET_REST("Quant", do_document_order_quant(alias, &document_order, 1)/do_product_coef(alias, &product));
        VALUE_SET_STR("Ei", do_strdup("шт."));
        VALUE_SET_MONEY("Sum", do_document_order_quant(alias, &document_order, 1)*
                               do_document_order_price(alias, &document_order, 9));

        do_template_cursor_first_run(line);

        status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_NEXT);
    }

    VALUE_SET_MONEY("Total", do_document_sum(alias, &document, 9));
    VALUE_SET_MONEY("TotalNDS", do_document_sum(alias, &document, 16010)+do_document_sum(alias, &document, 16020));
    VALUE_SET_INT("TotalStr", indx);
    value = double_money(do_document_sum(alias, &document, 9));
    sprintf(buffer, "%s руб.", value);
    VALUE_SET_STR("TotalMoney", do_strdup(buffer));
    VALUE_SET_STR("TotalString", do_real_in_words(alias, do_document_sum(alias, &document, 9), TRUE, TRUE));

    do_template_cursor_skip(cursor);
    do_template_cursor_run(cursor);

    if ( status == DO_ERROR ) {
        do_template_free(templ);
        return NULL;
    }
    return templ;
}
