#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <domino.h>
#include <../config/config.h>
#include <glib.h>


#define COUNT_ALIASES 3
#define COUNT_RANGE 6
#define RANGE_STEP 5

char *aliases[COUNT_ALIASES] = {"local.tta","local.chel","local.net"};
int procs[COUNT_RANGE] = {}

typedef struct {
    int unit;
    double sum_real[COUNT_RANGE+1];
    double sum_unreal[COUNT_RANGE+1];
} info_t;

gboolean get_analit(do_alias_t *alias, struct tm *date1, struct tm *date2, do_sort_list_t *list);
int cmp_func(info_t *i1, info_t *i2)
{
    return i1->unit - i2->unit;
}

int main(int argc, char *argv[])
{
    setlocale(LC_ALL,"");
    do_log_set_stdout();
    do_sort_list_t *list;
    int i;
    struct tm date1;
    struct tm date2;

    do_atodate(argv[1], &date1);
    do_atodate(argv[2], &date2);
    list = do_sort_list_new(TRUE, TRUE, (do_list_cmp_func)cmp_func, NULL);

    if ( !domino_init(NULL, FALSE, DO_PARAM_END) )
        exit(1);

    for ( i = 0; i < COUNT_ALIASES; i++ ) {
        do_alias_t *alias;

        if ( (alias = domino_alias_new(aliases[i])) == NULL )
            exit(1);
        if ( !do_alias_open(alias, TRUE) )
            exit(1);
        if ( !get_analit(alias, &date1, &date2, list) )
             exit(1);
        do_alias_free(alias);

    }
    printf("\"Аптека\";");
    for ( i = 0; i < COUNT_RANGE; i++ )
        printf("до %d;;", (i+1)*RANGE_STEP);
    printf("свыше;;\n");

    for ( i = 0; i < list->count; i++ ) {
        int j;
        info_t *node;
        node = list->list[i];
        printf("%d;", node->unit);
        for ( j = 0; j <= COUNT_RANGE; j++ )
            printf("%f;%f;", node->sum_real[j], node->sum_unreal[j]);
        printf("\n");
    }
    return 0;
}
gboolean get_document(do_alias_t *alias, document_rec_t *document, do_sort_list_t *list);

gboolean get_analit(do_alias_t *alias, struct tm *date1, struct tm *date2, do_sort_list_t *list)
{
    document_key1_t document_key1, key;
    document_rec_t document;
    BTI_CHAR dtype1[10];
    int status;

    do_text_set(alias, dtype1, "01");
    do_text_set(alias, document_key1.dtype, "ПТ");
    do_date_set(&document_key1.date, *date1);
    document_key1.time = 0;
    do_cpy(key.dtype, document_key1.dtype);
    do_date_set(&key.date, *date2);

    status = do_document_get1(alias, &document, &document_key1, DO_GET_GE);

    while ( status == DO_OK ) {
        if ( do_cmp(key.dtype, document_key1.dtype) ||
             document_key1.date > key.date
            ) break;

        if ( !do_cmp(document.data.dtype1, dtype1) ) {
            if ( !get_document(alias, &document, list) )
                return FALSE;
        }


        status = do_document_get1(alias, &document, &document_key1, DO_GET_NEXT);
    }

    if ( status == DO_ERROR )
        return FALSE;

    return TRUE;
}
gboolean get_document(do_alias_t *alias, document_rec_t *document, do_sort_list_t *list)
{
    document_order_key0_t document_order_key0;
    document_order_rec_t document_order;
    int status;
    int proc_real, proc_unreal;
    info_t info, *node;
    double sum;
    int indx;

    do_cpy(document_order_key0.dtype, document->data.dtype);
    do_cpy(document_order_key0.sklad, document->data.sklad);
    do_cpy(document_order_key0.document, document->data.document);
    document_order_key0.line = 0;
    info.unit = atoi(document->data.sklad);


    status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_GE);
    while ( status == DO_OK ) {
        if (
            do_cmp(document_order_key0.dtype, document->data.dtype) ||
            do_cmp(document_order_key0.sklad, document->data.sklad) ||
            do_cmp(document_order_key0.document, document->data.document)
            ) break;

        proc_real = ((do_document_order_price(alias, &document_order, 4) -
                    do_document_order_price(alias, &document_order, 2))/do_document_order_price(alias, &document_order, 2)) *100;
        proc_unreal = do_document_order_param_float(alias, &document_order, "НАЦЕНКА");
        sum  = do_document_order_price(alias, &document_order, 2)*do_document_order_quant(alias, &document_order, 1);

        if ( !do_sort_list_find(list, &indx, &info) ) {
            memset(&info.sum_real, 0, sizeof(info.sum_real));
            memset(&info.sum_unreal, 0, sizeof(info.sum_unreal));
            do_sort_list_insert_new(list, indx, &info, sizeof(info));
        }
        node = list->list[indx];
        if ( proc_real > COUNT_RANGE*RANGE_STEP )
            node->sum_real[COUNT_RANGE] += sum;
        else {
            for ( indx = 0; indx < COUNT_RANGE; indx++ )
                if ( proc_real <= (indx+1)*RANGE_STEP ) {
                    node->sum_real[indx] += sum;
                    break;
                }
        }
        if ( proc_unreal > COUNT_RANGE*RANGE_STEP )
            node->sum_unreal[COUNT_RANGE] += sum;
        else {
            for ( indx = 0; indx < COUNT_RANGE; indx++ )
                if ( proc_unreal <= (indx+1)*RANGE_STEP ) {
                    node->sum_unreal[indx] += sum;
                    break;
                }
        }

        status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_NEXT);
    }

    if ( status == DO_ERROR )
        return FALSE;


    return TRUE;

}
