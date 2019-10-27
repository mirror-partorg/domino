
include ../../common.mak

LDFLAGS = 
CFLAGS = -D_REENTRANT -pedantic
#$(PYTHONFLAGS)

SRCS = cdecode.c cencode.c

#SRCS_PY = log.c auth.c list.c mem.c data.c socket_io.c rpc_client.c rpc.c scanner.c thread.c \
#	proc_state.c service.c util.c $(misc)/iconv_.c charset.c numutils.c ole.c xls.c dolib_py.c db.c

VER = 1
NAME = libb64.so
#NAME_PY = _dolibc.so
DLL  = b64.dll
#PRE_NAME_PY = swig -python dolibc.i && cat dolib_py.include >dolib_py.c && cat dolibc_wrap.c >>dolib_py.c && rm -f cat dolibc_wrap.c

include ../../library.mak


