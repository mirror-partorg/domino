include ../../common.mak

LDFLAGS = $(LIBDOMINO) $(LIBDO) $(LIBICONV) $(LIBEXPAT) $(LIBPSQL) -lz
CFLAGS = -D_REENTRANT -DDEBUG

SRCS = dclubd.c dclub.c dclubinfo.c option.c \
       $(misc)/mproc_state.c $(misc)/csv_util.c $(misc)/mask.c $(misc)/iconv_.c

APP = dclubd

include ../../program.mak
