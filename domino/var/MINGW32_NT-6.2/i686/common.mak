CC = gcc 
TMP_DIR=var/MINGW32_NT-6.2/i686
MAKE=make
AM_MAKEFLAGS=CC=gcc
PYTHON_VERSION_3=3.4
AM_FLAGS=-Lc:/MinGw/../lib/bin -Lc:/MinGw/lib -L../../var/MINGW32_NT-6.2/i686/.libs -I../../var/MINGW32_NT-6.2/i686 \
-D_WIN32 -DBTI_WIN_32 -DWINSOCK \
-LC:/MinGW/msys/1.0/lib \
-Ic:/MinGw/include/gdk-pixbuf-2.0 \
-IC:/MinGW/msys/1.0/include \
-Ic:/MinGw/include/gtk-2.0 -Ic:/MinGw/include/cairo \
-Ic:/MinGw/include/webkitgtk-1.0 -Ic:/MinGw/include/libsoup-2.4\
-Ic:/MinGw/include/gdk -Ic:/MinGw/include/glib-2.0 \
-Ic:/MinGw/lib/glib-2.0/include -Ic:/MinGw/include/pango-1.0 \
-Ic:/MinGw/lib/gtk-2.0/include -Ic:/MinGw/include/atk-1.0 \
-Ic:/MinGw/include/gtksourceview-2.0 -Ic:/MinGw/include/loudmouth-1.0 -I../../include \
-Ic:/MinGw/include/libxml2 \
-Ic:/MinGw/../Python33/include \
-DEXPAT \
-DZLIB \
-DBINARY_UPDATE \
-DWIN_RELOAD_ACCEL \
-DUSE_LOUDMOUTH 
LIBWINSOCK=-lws2_32
LIBICONV=-liconv
LIBZ=-lz
LIBEXPAT=-lexpat
LIBFBCLIENT=-lfbclient_ms
LIBDO=-ldolib
LIBVLC=-lvlc
LIBDOMINO=-ldomino
LIBSOUP=-lsoup-2.4
LIBPSQL=-lw3btrv7
LIBXML2=-llibxml2
LIBEXPAT=-lexpat
LIBWEBKIT=-lwebkitgtk-1.0
LIBGTK=-lgtk-win32-2.0 -lgobject-2.0 -lglib-2.0 -lgdk-win32-2.0 -lpango-1.0 -lgthread-2.0 -lgdk_pixbuf-2.0 -lgio-2.0
LIBGLIB=-lglib-2.0
LIBLOUDMOUTH=-lloudmouth-1
WINAPP=-mms-bitfields -Wl,-subsystem,windows
WINMM=-lwinmm
misc = ../../misc
