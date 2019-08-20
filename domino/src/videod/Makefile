include ../../common.mak

LDFLAGS = $(LIBICONV) $(LIBLOUDMOUTH)  -lpq -lsmbclient $(LIBDO) $(LIBGLIB) -lpthread 
CFLAGS = -DHAVE_SYS_RESOURCE_H -DUSE_IM -DGLOG_OFF -D_REENTRANT -DUSE_POSTGRESQL \
		 $(GLIBCFLAGS) -I/usr/include/postgresql -DDEBUG $(SMBCLIENTCFLAGS)

SRCS = videod.c video.c videoinfo.c option.c \
       $(misc)/mproc_state.c $(misc)/storage.c $(misc)/send_error.c $(misc)/mythread.c

APP = videod

INSTALL=cp -f videod.init /etc/init.d/videod && chmod +x /etc/init.d/videod

include ../../program.mak
