
include ../../common.mak

LDFLAGS =  -ldomino -ldolib
CFLAGS1 = -D_REENTRANT -D_DEBUG -DUSE_IM
CFLAGS = -D_DEBUG1 -D_REENTRANT 

NAME = pydomino.so
DLL = pydomino.pyd
APP = pydomino

all: gen compile
gen:
	if [[ ! -d doc ]] ; then mkdir doc ; fi
	if [[ ! -d var ]] ; then mkdir var ; fi
	if [[ ! -f var/_info.txt ]] ; then /usr/bin/env python3 script/generate.py ; fi
include ../../pylibrary.mak
