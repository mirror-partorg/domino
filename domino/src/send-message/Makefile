
include ../../common.mak

LDFLAGS =  $(LIBDO)  $(LIBLOUDMOUTH) $(LIBGLIB) $(LIBWINSOCK) $(LIBICONV)
CFLAGS =  $(GLIBCFLAGS) -DGLOG_OFF -DUSE_IM

SRCS = main.c \
	$(misc)/send_error.c
SRCS_1 = main_utf.c \
  	$(misc)/send_error.c  $(misc)/iconv_.c

APP = send-error-message

APP_1 = send-error-message_utf

include ../../program.mak


