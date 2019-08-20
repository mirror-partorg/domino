
include ../../common.mak

LDFLAGS =  $(LIBDOMINO) $(LIBDO)  $(LIBLOUDMOUTH) $(LIBGLIB) $(LIBWINSOCK)
CFLAGS =  $(GLIBCFLAGS) -DGLOG_OFF -DUSE_LOUDMOUNT -g

SRCS = main.c grlspl.c \
	$(misc)/send_error.c $(misc)/utf8util.c

APP = grlspl-load

include ../../program.mak


