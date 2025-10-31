CC = gcc
LINK = g++
TMP_DIR=../../../../var/Linux/Gentoo/mirror-book/x86_64
DOMINO_ETC=/usr/local/domino/etc
DOMINO_BIN=/usr/local/domino/bin
DOMINO_LOG=/usr/local/domino/log
DOMINO_LIB=/usr/local/domino/lib
DOMINO_TEMPLATE=/usr/local/domino/template
DOMINO_SHARE=/usr/local/domino/share
MAKE=make
AM_MAKEFLAGS=CC=gcc
AM_FLAGS=-L$(TMP_DIR)/.libs -I/usr/include/loudmouth-1.0 -I../../lib/ffmpeg -I../../include -D_LINUX -DHAVE_SYSLOG -L/usr/local/psql/lib -L/usr/local/psql/lib64 -DBTI_LINUX_64 -I../../var/Linux/Gentoo/mirror-book/x86_64\
-DZLIB \
-DDOMINO78 \
-DCUPS \
-DUSE_NCURSES \

OPT_FLAGS=-ggdb
LIBCUPS=-lcups
LIBZ=-lz
LIBNCURSES=-lncurses
LIBFBCLIENT=-lfbclient
LIBDO=-ldolib
LIBDOMINO=-ldomino
LIBPSQL=-lpsqlmif
LIBPTHREAD=-lpthread
WEBKITCFLAG=
LIBWEBKIT=
JSONGLIBCFLAG=-I/usr/include/json-glib-1.0 -I/usr/lib64/libffi/include -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include -I/usr/include/libmount -I/usr/include/blkid -I/usr/include/sysprof-6 -pthread
LIBJSONGLIB=-ljson-glib-1.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0
LIBSQLITE3=-lsqlite3
LIBXML2=-lxml2
LIBAV=-lavcodec -lavformat -lavdevice -lswscale -lavfilter -lavutil -lswresample -L../../lib/ffmpeg/libavcodec -L../../lib/ffmpeg/libavutil  -L../../lib/ffmpeg/libavdevice -L../../lib/ffmpeg/libswscale  -L../../lib/ffmpeg/libswresample  -L../../lib/ffmpeg/libavformat -L../../lib/ffmpeg/libavfilter  -Wl,-rpath,../../lib/ffmpeg/libavutil:../../lib/ffmpeg/libavformat:../../lib/ffmpeg/libswresample
SMBCLIENTCFLAGS=-I/usr/include/samba-4.0
XML2CFLAGS=-I/usr/include/libxml2
LIBSOUP=-lsoup-2.4
LIBPQ=
PQCFLAGS=
GET_VERSION = `cat version.h | grep -I VERSION | cut -d' ' -f3 | egrep '^"[0-9]+.[0-9]+.[0-9]+"' | cut -d'"' -f2`
misc = ../../misc
SUBDIRS=etc  	lib/dolib lib/domino  lib/pydomino 	src/ctrlpasswd src/domino src/exchanged src/send-file src/sync src/tmcck src/handbook-text   	src/period src/ddfutil src/grlspl-load template share src/get-stock
PYTHON_VERSION_3=3.13
PYTHON_CFLAGS_3=-I/usr/include/python3.13
PYTHON_LIBS_3=
PYTHON_VERSION_3=3.13
PYTHON_DIR_3=/usr/lib/python3.13/site-packages
