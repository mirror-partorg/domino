CC = gcc
LINK = g++
TMP_DIR=../../../../var/Linux/Ubuntu/trusty/i686
DOMINO_ETC=/usr/local/domino/etc
DOMINO_BIN=/usr/local/domino/bin
DOMINO_LOG=/usr/local/domino/log
DOMINO_LIB=/usr/local/domino/lib
DOMINO_TEMPLATE=/usr/local/domino/template
DOMINO_SHARE=/usr/local/domino/share
MAKE=make
AM_MAKEFLAGS=CC=gcc
AM_FLAGS=-L$(TMP_DIR)/.libs -I/usr/include/loudmouth-1.0 -I../../lib/ffmpeg -I../../include -D_LINUX -DHAVE_SYSLOG -L/usr/local/psql/lib -L/usr/local/psql/lib64 -DBTI_LINUX -I../../var/Linux/Ubuntu/trusty/i686 \
-I/usr/include/libsoup-2.4 \
-DEXPAT \
-DZLIB \
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
LIBSOUP=-lsoup-2.4
WEBKITCFLAG=-pthread -I/usr/include/webkitgtk-3.0 -I/usr/include/glib-2.0 -I/usr/lib/i386-linux-gnu/glib-2.0/include -I/usr/include/gtk-3.0 -I/usr/include/libsoup-2.4 -I/usr/include/atk-1.0 -I/usr/include/at-spi2-atk/2.0 -I/usr/include/pango-1.0 -I/usr/include/gio-unix-2.0/ -I/usr/include/cairo -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/harfbuzz -I/usr/include/freetype2 -I/usr/include/pixman-1 -I/usr/include/libpng12 -I/usr/include/libxml2 
LIBWEBKIT=-lwebkitgtk-3.0 -lgtk-3 -lsoup-2.4 -ljavascriptcoregtk-3.0 -lgdk-3 -latk-1.0 -lpangocairo-1.0 -lgdk_pixbuf-2.0 -lcairo-gobject -lpango-1.0 -lcairo -lgio-2.0 -lgobject-2.0 -lglib-2.0 
LIBSQLITE3=-lsqlite3
JSONGLIBCFLAG=-pthread -I/usr/include/json-glib-1.0 -I/usr/include/glib-2.0 -I/usr/lib/i386-linux-gnu/glib-2.0/include  
LIBJSONGLIB=-ljson-glib-1.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0  
LIBXML2=-lxml2  
LIBAV=-lavcodec -lavformat -lavdevice -lswscale -lavfilter -lavutil -lswresample -L../../lib/ffmpeg/libavcodec -L../../lib/ffmpeg/libavutil  -L../../lib/ffmpeg/libavdevice -L../../lib/ffmpeg/libswscale  -L../../lib/ffmpeg/libswresample  -L../../lib/ffmpeg/libavformat -L../../lib/ffmpeg/libavfilter  -Wl,-rpath,../../lib/ffmpeg/libavutil:../../lib/ffmpeg/libavformat:../../lib/ffmpeg/libswresample
SMBCLIENTCFLAGS=-I/usr/include/samba-4.0  
LIBGTK=-lgtk-3 -lgdk-3 -latk-1.0 -lgio-2.0 -lpangocairo-1.0 -lgdk_pixbuf-2.0 -lcairo-gobject -lpango-1.0 -lcairo -lgobject-2.0 -lglib-2.0  
LIBGLIB=-lglib-2.0  
LIBPOPPLER=
POPPLERCFLAGS=
XML2CFLAGS=-I/usr/include/libxml2  
GTKCFLAGS=-pthread -I/usr/include/gtk-3.0 -I/usr/include/atk-1.0 -I/usr/include/at-spi2-atk/2.0 -I/usr/include/pango-1.0 -I/usr/include/gio-unix-2.0/ -I/usr/include/cairo -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/glib-2.0 -I/usr/lib/i386-linux-gnu/glib-2.0/include -I/usr/include/harfbuzz -I/usr/include/freetype2 -I/usr/include/pixman-1 -I/usr/include/libpng12  
LIBPQ=-lpq  
PQCFLAGS=-I/usr/include/postgresql  
GLIBCFLAGS=-I/usr/include/glib-2.0 -I/usr/lib/i386-linux-gnu/glib-2.0/include  
LIBEXPAT=-lexpat
GET_VERSION = `cat version.h | grep -I VERSION | cut -d' ' -f3 | egrep '^"[0-9]+.[0-9]+.[0-9]+"' | cut -d'"' -f2`
misc = ../../misc
SUBDIRS=etc  	lib/dolib lib/domino  lib/pydomino 	src/ctrlpasswd src/domino src/exchanged src/send-file src/sync src/tmcck src/handbook-text   	src/period src/ddfutil src/grlspl-load template share src/get-stock
PYTHON_VERSION_2=2.7
PYTHON_CFLAGS_2=-I/usr/include/python2.7 -I/usr/include/i386-linux-gnu/python2.7
PYTHON_LIBS_2=-lpython2.7
PYTHON_VERSION_2=2.7
PYTHON_DIR_2=/usr/lib/python2.7/dist-packages
PYTHON_VERSION_3=3.4
PYTHON_CFLAGS_3=-I/usr/include/python3.4m -I/usr/include/i386-linux-gnu/python3.4m
PYTHON_LIBS_3=-lpython3.4m
PYTHON_VERSION_3=3.4
PYTHON_DIR_3=/usr/lib/python3/dist-packages
