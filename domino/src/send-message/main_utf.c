#include <stdio.h>
#include <stdlib.h>
#include <dolib.h>
#include "../misc/send_error.h"
#include "../misc/iconv_.h"


int main(int argc, char *argv[])
{
    char *str;
    char *buf;	
    str = get_locale_lang();
    iconv_t conv = iconv_open("UTF-8", str);

    do_log_set_stdout(); 	
    if ( argc != 2 ) {
	printf("usage %s <message>", argv[0]);
	exit(1);
    }
    buf = iconv_(conv, argv[1]);
    send_error_message(buf);
    return 0;
}
