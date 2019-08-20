include ../../common.mak

LDFLAGS =  $(LIBDO) $(LIBDOMINO) $(LIBGTK) $(LIBXML2) -mms-bitfields -Wl,-subsystem,windows

CFLAGS = -D_REENTRANT -DUSE_GTK -DUSE_UTF8 $(GTKCFLAGS) $(XML2CFLAGS) 


SRCS = main.c do_link_dialog.c do_message.c do_progress_info.c do_scan.c document.c exchange.c misc.c partner.c

APP = exchange-1c

include ../../program.mak
