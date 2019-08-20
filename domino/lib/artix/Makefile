
include ../../common.mak

LDFLAGS = $(LIBPTHEAD) $(LIBWINSOCK) $(LIBICONV) $(LIBDO)
CFLAGS = -D_REENTRANT 

SRCS = artix.c  artix_info.c $(misc)/cash_key_command.c \
       $(misc)/iconv_.c $(misc)/mthrift.c

VER  = 4
NAME = libartix.so
DLL  = artix.dll


include ../../library.mak

