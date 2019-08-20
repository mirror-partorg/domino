
include ../../common.mak

LDFLAGS = $(LIBLOUDMOUTH) $(LIBGLIB) $(LIBWINSOCK) $(LIBICONV)
CFLAGS =  $(GLIBCFLAGS) -DGLOG_OFF -DUSE_IM

SRCS = main.c send_error.c

APP = send-error-message

include ../../program.mak


