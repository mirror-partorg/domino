#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dolib.h>
#include <domino.h>
#include "../misc/forms.h"
#include "../misc/print.h"

#include <time.h>
#include <locale.h>
#include <stddef.h>

int sklad_int = 1;
int do1 = 1;
char *host;

static int print_doc_in(do_alias_t *alias, document_key0_t *document_key0);
int main(int argc, char *argv[])
{
    setlocale(LC_ALL,"");
    do_log_set_stdout();
    do_alias_t *alias;

    if ( argc < 5 ) {
	fprintf(stderr, "Usage %s <alias_name> <document> <host> <kind> \n", argv[0]);
	exit(1);
    }

    if (!domino_init(NULL, FALSE, DO_PARAM_END))
        exit(1);

    if ((alias = domino_alias_new(argv[1])) == NULL)
        exit(1);
    if ( !do_alias_open(alias, TRUE))
        exit(1);
    document_key0_t document_key0;
    if ( !do_util_str_to_document_key0(alias, argv[2], &document_key0) ) {
	do_log(LOG_ERR, "document \"%s\" incorrect", argv[2]);
	exit(1);
    }

    host = do_strdup(argv[3]);
    do1 = atoi(argv[4]);

    print_doc_in(alias, &document_key0);


    do_alias_free(alias);
    return 0;
}
static int print_doc_in(do_alias_t *alias, document_key0_t *document_key0)
{
    do_template_t *tm;
    document_rec_t document;
//    const char *host = "programmer";
    int found;

    if ( do1 == 1 )
	tm = do_form_torg12(alias, document_key0);
    else
    if ( do1 == 2 )
	tm = do_form_sf(alias, document_key0);
    else
    if ( do1 == 3 )
	tm = do_form_protocol(alias, document_key0, &found);
    else
    if ( do1 == 4 )
    {
	do_document_get0(alias, &document, document_key0, DO_GET_EQUAL);
	tm = do_form_invoice(alias, document_key0, atoi(document.data.p_doc) + 2476);
    }
    else
    {
	do_log(LOG_ERR, "unsupport %d", do1);
	exit(1);
    }

    printf("print \"%s\"\n", do_util_document_key0_to_str(alias, document_key0));
    char buf[1024];
    sprintf(buf,"doc%s.html", do_text(alias, document_key0->document));
    //do_template_save_to_file(tm, buf);
    if ( !print_html(host, do_template_text(tm)) ) {
    	do_log(LOG_ERR, "can't print %s", do_util_document_key0_to_str(alias, document_key0));
    	exit(1);
    }
    DO_SEC_SLEEP(10);
    return 1;
}
