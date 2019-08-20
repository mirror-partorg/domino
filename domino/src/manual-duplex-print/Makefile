
include ../../common.mak

LDFLAGS =  $(LIBGTK) $(LIBPOPPLER) $(LIBCUPS)
CFLAGS =  $(GTKCFLAGS) $(POPPLERCFLAGS) -I/usr/include/gtk-2.0/unix-print -I/usr/include/gtk-unix-print-2.0

SRCS = main.c 
SRCS_1 = barcode.c 

APP = pdf-print
APP_1 = barcode-print

include ../../program.mak


