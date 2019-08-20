CC = gcc
TMP_DIR=var/Linux/CentOS/Final/i686
DOMINO_ETC=/usr/local/domino/etc
DOMINO_BIN=/usr/local/domino/bin
DOMINO_LOG=/usr/local/domino/log
DOMINO_LIB=/usr/local/domino/lib
DOMINO_TEMPLATE=/usr/local/domino/template
DOMINO_SHARE=/usr/local/domino/share
MAKE=make
AM_MAKEFLAGS=CC=gcc
AM_FLAGS=-L../../var/Linux/CentOS/Final/i686/.libs -I../../lib/ffmpeg -I../../include -D_LINUX -DHAVE_SYSLOG -L/usr/local/psql/lib -L/usr/local/psql/lib64 -DBTI_LINUX -I../../var/Linux/CentOS/Final/i686\
-DZLIB \
-DUSE_NCURSES \
-DDOMINO78 
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
LIBXML2=
LIBAV=-lavcodec -lavformat -lavdevice -lswscale -lavfilter -lavutil -lswresample -L../../lib/ffmpeg/libavcodec -L../../lib/ffmpeg/libavutil  -L../../lib/ffmpeg/libavdevice -L../../lib/ffmpeg/libswscale  -L../../lib/ffmpeg/libswresample  -L../../lib/ffmpeg/libavformat -L../../lib/ffmpeg/libavfilter  -Wl,-rpath,../../lib/ffmpeg/libavutil:../../lib/ffmpeg/libavformat:../../lib/ffmpeg/libswresample
SMBCLIENTCFLAGS=
LIBGTK=
LIBGLIB=
LIBPOPPLER=
POPPLERCFLAGS=
XML2CFLAGS=
GTKCFLAGS=
LIBPQ=
PQCFLAGS=
GLIBCFLAGS=
LIBLOUDMOUTH=
LIBEXPAT=
GET_VERSION = `cat version.h | grep -I VERSION | cut -d' ' -f3 | egrep '^"[0-9]+.[0-9]+.[0-9]+"' | cut -d'"' -f2`
misc = ../../misc
SUBDIRS=etc  	lib/dolib lib/domino  lib/pydomino 	src/ctrlpasswd src/domino src/exchanged src/send-file src/sync src/tmcck src/handbook-text   	src/period src/ddfutil src/grlspl-load template share src/get-stock
PYTHON_VERSION_2=
PYTHON_CFLAGS_2=-I/usr/include/python2.4
PYTHON_LIBS_2=
PYTHON_VERSION_2=
PYTHON_DIR_2=/usr/lib/python2.4/site-packages
PYTHON_VERSION_3=3.1
PYTHON_CFLAGS_3=-I../../var/Linux/CentOS/Final/i686/python-3.1.5/Include -I../../var/Linux/CentOS/Final/i686/python-3.1.5/
PYTHON_LIBS_3=
PYTHON_VERSION_3=3.1
PYTHON_DIR_3=/usr/local/lib/python3.1/site-packages
