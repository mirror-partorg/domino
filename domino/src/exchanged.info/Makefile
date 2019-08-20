
include ../../common.mak

LDFLAGS = $(LIBDOMINO) $(LIBDO) $(LIBICONV) $(LIBLOUDMOUTH) -L../loudmouth-1.0.1/loudmouth/.libs
CFLAGS = -D_REENTRANT -I../loudmouth-1.0.1 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include

SRCS = exchanged.c exchangeinfo.c option.c  \
       $(misc)/iconv_.c $(misc)/mproc_state.c 
       
#SRCS_1 = main.c send_error.c
       

APP = exchanged
#APP_1 = send-message

include ../../program.mak

