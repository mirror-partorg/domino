#include <stdio.h>
#include <stdlib.h>
#include <dolib.h>
#include "../misc/send_error.h"


int main(int argc, char *argv[])
{
    do_log_set_stdout(); 	
    if ( argc != 2 ) {
	printf("usage %s <message>", argv[0]);
	exit(1);
    }
    send_error_message(argv[1]);
    return 0;
}
