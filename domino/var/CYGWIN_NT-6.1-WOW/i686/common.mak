CC = gcc
TMP_DIR=var/CYGWIN_NT-6.1-WOW/i686
DOMINO_ETC=/usr/local/domino/etc
DOMINO_BIN=/usr/local/domino/bin
DOMINO_LOG=/usr/local/domino/log
DOMINO_LIB=/usr/local/domino/lib
DOMINO_TEMPLATE=/usr/local/domino/template
DOMINO_SHARE=/usr/local/domino/share
MAKE=make
AM_MAKEFLAGS=CC=gcc
AM_FLAGS=-L../../var/CYGWIN_NT-6.1-WOW/i686/.libs -I/usr/include/loudmouth-1.0 -I../../lib/ffmpeg -I../../include -D_LINUX -DHAVE_SYSLOG -L/usr/local/psql/lib -L/usr/local/psql/lib64 -DBTI_LINUX -I../../var/CYGWIN_NT-6.1-WOW/i686\
-DEXPAT \
-DZLIB \
-DUSE_NCURSES \
-DUSE_LOUDMOUTH \

OPT_FLAGS=-s -O2
LIBZ=-lz
LIBNCURSES=-lncurses
LIBFBCLIENT=-lfbclient
LIBDO=-ldolib
LIBDOMINO=-ldomino
LIBPSQL=-lpsqlmif
LIBPTHREAD=-lpthread
WEBKITCFLAG=
LIBWEBKIT=
LIBXML2=
LIBAV=-lavcodec -lavdevice -lswscale -lavfilter  -L../../lib/ffmpeg/libavcodec -L../../lib/ffmpeg/libavutil  -L../../lib/ffmpeg/libavdevice -L../../lib/ffmpeg/libswscale  -L../../lib/ffmpeg/libswresample  -L../../lib/ffmpeg/libavfilter  -Wl,-rpath,../../lib/ffmpeg/libavutil:../../lib/ffmpeg/libavformat:../../lib/ffmpeg/libswresample
LIBGTK=
LIBEV=
EVCFLAGS=
LIBGLIB=
XML2CFLAGS=
GTKCFLAGS=
LIBPQ=
PQCFLAGS=
GLIBCFLAGS=
LIBLOUDMOUTH=-lloudmouth-1
LIBEXPAT=-lexpat
GET_VERSION = `cat version.h | grep -I VERSION | cut -d' ' -f3 | egrep '^"[0-9]+.[0-9]+.[0-9]+"' | cut -d'"' -f2`
misc = ../../misc
SUBDIRS=etc  	lib/dolib lib/domino  lib/pydomino 	src/ctrlpasswd src/domino src/exchanged src/send-file src/sync src/tmcck src/handbook-text   	src/period src/ddfutil src/grlspl-load template share src/get-stock
