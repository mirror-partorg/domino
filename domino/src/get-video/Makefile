
include ../../common.mak

LDFLAGS = $(LIBDO) $(LIBICONV) $(LIBLOUDMOUTH) $(LIBGLIB) -lpq -lsmbclient -lpthread \
		  $(LIBAV) $(LIBZ)
CFLAGS = -DHAVE_SYS_RESOURCE_H -DUSE_IM -DGLOG_OFF -D_REENTRANT -DUSE_POSTGRESQL \
		 -DUSE_AVFILTER -DUSE_LIBAV1  $(GLIBCFLAGS) -I/usr/include/postgresql -I/usr/include/samba-4.0/

SRCS = main.c libav.c $(misc)/storage.c option.c $(misc)/mythread.c $(misc)/send_error.c $(misc)/mproc_state.c
APP  = get-video

include ../../program.mak
