include ../../common.mak


LDFLAGS =  $(LIBDO) $(LIBDOMINO) $(LIBLOUDMOUTH) $(LIBGLIB) $(LIBWINSOCK) $(LIBPSQL) $(LIBEXPAT)

CFLAGS = -D_REENTRANT $(GLIBCFLAGS) -DGLOG_OFF -DUSE_OLD_BARCODE -DUSE_BTI \
         -DSEND_ERROR_ONLY_WORK_TIME -DDEBUG_READ_ENUM_ -DHAVY_DEBUG_ -DDEB_EXIT1_

SRCS = syncd.c sync_info.c sync_server.c option.c replic.c \
       $(misc)/mproc_state.c $(misc)/send_error.c util.c

SRCS_1 = sync_client.c replic.c  option_client.c \
       $(misc)/mproc_state.c $(misc)/send_error.c util.c

APP = syncd
APP_1 = sync-client


INSTALL=cp -f syncd.init /etc/init.d/syncd && chmod +x /etc/init.d/syncd  
UNINSTALL=rm -f $(DOMINO_BIN)/sync-unit && rm -f $(DOMINO_BIN)/sync && rm -f $(DOMINO_ETC)/syncd.conf && rm -f /etc/init.d/syncd

include ../../program.mak

