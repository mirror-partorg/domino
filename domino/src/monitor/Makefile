
LIBDTI=-lpsqldti
include ../../common.mak


LDFLAGS = $(LIBDTI) $(LIBDO) $(LIBLOUDMOUTH) $(LIBGLIB) $(LIBWINSOCK) $(LIBICONV)
CFLAGS =  $(GLIBCFLAGS) -DGLOG_OFF -DUSE_IM -g

SRCS = main.c $(misc)/send_error.c
SRCS_1 = clear.c 

APP = check-pervasive
APP_1 = clear-transaction-users

include ../../program.mak


