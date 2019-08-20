CC = gcc
TMP_DIR=var/Linux/Gentoo/database/x86_64
DOMINO_ETC=/usr/local/domino/etc
DOMINO_BIN=/usr/local/domino/bin
DOMINO_LOG=/usr/local/domino/log
DOMINO_LIB=/usr/local/domino/lib
DOMINO_TEMPLATE=/usr/local/domino/template
DOMINO_SHARE=/usr/local/domino/share
MAKE=make
AM_MAKEFLAGS=CC=gcc
AM_FLAGS=-L../../var/Linux/Gentoo/database/x86_64/.libs -I/usr/include/loudmouth-1.0 -I../../lib/ffmpeg -I../../include -D_LINUX -DHAVE_SYSLOG -L/usr/local/psql/lib -L/usr/local/psql/lib64 -DBTI_LINUX_64 -I../../var/Linux/Gentoo/database/x86_64\
-DEXPAT \
-DZLIB \
-DUSE_NCURSES \
-DUSE_IM \

OPT_FLAGS=-ggdb
LIBZ=-lz
LIBNCURSES=-lncurses
LIBFBCLIENT=-lfbclient
LIBDO=-ldolib
LIBDOMINO=-ldomino
LIBPSQL=-lpsqlmif
LIBPTHREAD=-lpthread
WEBKITCFLAG=
LIBWEBKIT=
LIBXML2=-lxml2
LIBAV=-lavcodec -lavformat -lavdevice -lswscale -lavfilter -lavutil -lswresample -L../../lib/ffmpeg/libavcodec -L../../lib/ffmpeg/libavutil  -L../../lib/ffmpeg/libavdevice -L../../lib/ffmpeg/libswscale  -L../../lib/ffmpeg/libswresample  -L../../lib/ffmpeg/libavformat -L../../lib/ffmpeg/libavfilter  -Wl,-rpath,../../lib/ffmpeg/libavutil:../../lib/ffmpeg/libavformat:../../lib/ffmpeg/libswresample
SMBCLIENTCFLAGS=-I/usr/include/samba-4.0
LIBGTK=
LIBGLIB=-lglib-2.0
LIBPOPPLER=
POPPLERCFLAGS=
XML2CFLAGS=-I/usr/include/libxml2
GTKCFLAGS=
LIBPQ=-L/usr/lib64/postgresql-9.6/lib64 -lpq
PQCFLAGS=-I/usr/include/postgresql-9.6
GLIBCFLAGS=-I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include
LIBLOUDMOUTH=-lloudmouth-1
LIBEXPAT=-lexpat
GET_VERSION = `cat version.h | grep -I VERSION | cut -d' ' -f3 | egrep '^"[0-9]+.[0-9]+.[0-9]+"' | cut -d'"' -f2`
misc = ../../misc
SUBDIRS=etc  	lib/dolib lib/domino  lib/pydomino 	src/ctrlpasswd src/domino src/exchanged src/send-file src/sync src/tmcck src/handbook-text   	src/period src/ddfutil src/grlspl-load template share src/get-stock
PYTHON_VERSION_2=2.7
PYTHON_CFLAGS_2=-I/usr/include/python2.7
PYTHON_LIBS_2=-lpython2.7
PYTHON_VERSION_2=2.7
PYTHON_DIR_2=/usr/lib64/python2.7/site-packages
PYTHON_VERSION_3=3.5
PYTHON_CFLAGS_3=-I/usr/include/python3.5m
PYTHON_LIBS_3=-lpython3.5m
PYTHON_VERSION_3=3.5
PYTHON_DIR_3=/usr/lib64/python3.5/site-packages
