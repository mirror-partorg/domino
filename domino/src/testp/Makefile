
include ../../common.mak

LDFLAGS = `pkg-config python-2.7 --libs` -ldomino 
CFLAGS = -D_REENTRANT `pkg-config python-2.7 --cflags`  -D_DEBUG

SRCS = template.c

VER = 1
NAME = domino.so
DLL  = testp.dll

include ../../library.mak


