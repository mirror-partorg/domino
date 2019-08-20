
include ../../common.mak

LDFLAGS = $(LIBDO) $(LIBICONV) $(LIBLOUDMOUTH) $(LIBGLIB) -lpq -lsmbclient \
		  $(LIBVLC)
CFLAGS = -DHAVE_SYS_RESOURCE_H -DUSE_IM -DGLOG_OFF -D_REENTRANT -DUSE_POSTGRESQL \
		  $(GLIBCFLAGS) $(LIVVLCFLAGS) -I/usr/include/postgresql 


SRCS = main.c vlc_stream.c $(misc)/storage.c option.c $(misc)/mythread.c $(misc)/send_error.c $(misc)/mproc_state.c
APP  = get-stream

include ../../program.mak
