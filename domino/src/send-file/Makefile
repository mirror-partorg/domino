include ../../common.mak


LDFLAGS =  $(LIBDO) $(LIBZ) $(LIBGCRIPT) $(LIBLOUDMOUTH) $(LIBWINSOCK) $(LIBGLIB) $(WINAPP) 

CFLAGS = $(GLIBCFLAGS) -DGLOG_OFF 


SRCS = main.c  $(misc)/send_error.c

APP = send-file

include ../../program.mak
