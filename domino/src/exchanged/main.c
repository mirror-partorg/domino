#include <stdio.h>
#include <stdlib.h>
#include "send_error.h"


int main(int argc, char *argv[])
{
    if ( argc != 2 ) {
	printf("usage %s <message>", argv[0]);
	exit(1);
    }
    send_error_message_xmpp(argv[1]);
    return 0;
}
