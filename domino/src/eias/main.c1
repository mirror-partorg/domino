#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dolib.h>
#include <domino.h>

#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>

#include <time.h>
#include <locale.h>
#include <stddef.h>
#ifdef _LINUX
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#define COEF 1.003
#define FIRM_OUT "03.219"

int main(int argc, char *argv[])
{
    setlocale(LC_ALL,"");
    do_log_set_stdout();
    do_alias_t *alias_old;
    char *document;
    document_key0_t document_key0;

    if ( argc < 3 ) {
        fprintf(stderr, "Usage %s <alias> <document>\n", argv[0]);
        exit(1);
    }
    if ( !domino_init(NULL, FALSE, DO_PARAM_END) )
        exit(1);

    if ( (alias_old = domino_alias_new(argv[1])) == NULL )
        exit(1);
    if ( !do_alias_open(alias_old, TRUE) )
        exit(1);

    document = argv[2];
    if ( !do_util_str_to_document_key0(alias_old, document, &document_key0) ) {
        do_log(LOG_ERR, "doc \"%s\" not correct", document);
        exit(1);
    }
    if ( !do_deaccept(alias_old, &document_key0, DO_DEACCEPT_DEFAULT, NULL) )
	exit(1);
    if ( !do_accept(alias_old, &document_key0, DO_ACCEPT_DEFAULT, NULL) )
	exit(1);
    do_alias_free(alias_old);
    return 0;
}

