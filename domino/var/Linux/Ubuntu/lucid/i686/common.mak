CC = gcc
TMP_DIR=var/Linux/Ubuntu/lucid/i686
DOMINO_ETC=/usr/local/domino/etc
DOMINO_BIN=/usr/local/domino/bin
DOMINO_LOG=/usr/local/domino/log
DOMINO_LIB=/usr/local/domino/lib
DOMINO_TEMPLATE=/usr/local/domino/template
DOMINO_SHARE=/usr/local/domino/share
MAKE=make
AM_MAKEFLAGS=CC=gcc
AM_FLAGS=-L../../var/Linux/Ubuntu/lucid/i686/.libs -I/usr/include/loudmouth-1.0 -I../../lib/ffmpeg -I../../include -D_LINUX -DHAVE_SYSLOG -L/usr/local/psql/lib -L/usr/local/psql/lib64 -DBTI_LINUX -I../../var/Linux/Ubuntu/lucid/i686\
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
WEBKITCFLAG=-pthread -I/usr/local/include/webkit-1.0 -I/usr/local/include/glib-2.0 -I/usr/local/lib/glib-2.0/include -I/usr/local/include/libsoup-2.4 -I/usr/local/include/cairo -I/usr/local/include/gio-unix-2.0/ -I/usr/local/include/pixman-1 -I/usr/include/gtk-2.0 -I/usr/lib/gtk-2.0/include -I/usr/include/atk-1.0 -I/usr/include/pango-1.0 -I/usr/include/freetype2 -I/usr/include/libpng12 -I/usr/include/libxml2  
LIBWEBKIT=-pthread -L/usr/local/lib -lwebkitgtk-1.0 -lgtk-x11-2.0 -lsoup-2.4 -lgdk-x11-2.0 -latk-1.0 -lpangoft2-1.0 -lgdk_pixbuf-2.0 -lm -lpangocairo-1.0 -lcairo -lpango-1.0 -lfreetype -lfontconfig -lgmodule-2.0 -lrt -lgio-2.0 -lgobject-2.0 -lglib-2.0  
LIBXML2=-lxml2  
LIBAV=-lavcodec -lavformat -lavdevice -lswscale -lavfilter -lavutil -lswresample -L../../lib/ffmpeg/libavcodec -L../../lib/ffmpeg/libavutil  -L../../lib/ffmpeg/libavdevice -L../../lib/ffmpeg/libswscale  -L../../lib/ffmpeg/libswresample  -L../../lib/ffmpeg/libavformat -L../../lib/ffmpeg/libavfilter  -Wl,-rpath,../../lib/ffmpeg/libavutil:../../lib/ffmpeg/libavformat:../../lib/ffmpeg/libswresample
SMBCLIENTCFLAGS=
LIBGTK=-pthread -L/usr/local/lib -lgtk-x11-2.0 -lgdk-x11-2.0 -latk-1.0 -lgio-2.0 -lpangoft2-1.0 -lgdk_pixbuf-2.0 -lm -lpangocairo-1.0 -lcairo -lpango-1.0 -lfreetype -lfontconfig -lgobject-2.0 -lgmodule-2.0 -lrt -lglib-2.0  
LIBGLIB=-L/usr/local/lib -lglib-2.0  
XML2CFLAGS=-I/usr/include/libxml2  
GTKCFLAGS=-pthread -I/usr/local/include/cairo -I/usr/local/include/gio-unix-2.0/ -I/usr/local/include/glib-2.0 -I/usr/local/lib/glib-2.0/include -I/usr/local/include/pixman-1 -I/usr/include/gtk-2.0 -I/usr/lib/gtk-2.0/include -I/usr/include/atk-1.0 -I/usr/include/pango-1.0 -I/usr/include/freetype2 -I/usr/include/libpng12  
LIBPQ=
PQCFLAGS=
GLIBCFLAGS=-I/usr/local/include/glib-2.0 -I/usr/local/lib/glib-2.0/include  
LIBLOUDMOUTH=-lloudmouth-1
LIBEXPAT=-lexpat
GET_VERSION = `cat version.h | grep -I VERSION | cut -d' ' -f3 | egrep '^"[0-9]+.[0-9]+.[0-9]+"' | cut -d'"' -f2`
misc = ../../misc
SUBDIRS=etc  	lib/dolib lib/domino  lib/pydomino 	src/ctrlpasswd src/domino src/exchanged src/send-file src/sync src/tmcck src/handbook-text   	src/period src/ddfutil src/grlspl-load template share src/get-stock
PYTHON_VERSION_2=2.6
PYTHON_CFLAGS_2=-I/usr/include/python2.6
PYTHON_LIBS_2=-lpython2.6
PYTHON_VERSION_2=2.6
PYTHON_DIR_2=/usr/lib/python2.6/dist-packages
PYTHON_VERSION_3=3.1
PYTHON_CFLAGS_3=-I/usr/include/python3.1
PYTHON_LIBS_3=-lpython3.1
PYTHON_VERSION_3=3.1
PYTHON_DIR_3=/usr/lib/python3.1/dist-packages
