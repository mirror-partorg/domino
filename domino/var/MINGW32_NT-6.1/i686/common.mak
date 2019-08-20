CC = gcc 
TMP_DIR=var/MINGW32_NT-6.1/i686
MAKE=make
AM_MAKEFLAGS=CC=gcc
PYTHON_VERSION_3=3.4
AM_FLAGS=-Lc:/Dev/MinGw/../lib/bin -Lc:/Dev/MinGw/lib -L../../var/MINGW32_NT-6.1/i686/.libs -I../../var/MINGW32_NT-6.1/i686 \
-D_WIN32 -DBTI_WIN_32 -DWINSOCK \
-Ic:/Dev/MinGw/include/gdk-pixbuf-2.0 \
-Ic:/Dev/MinGw/include/gtk-2.0 -Ic:/Dev/MinGw/include/cairo \
-Ic:/Dev/MinGw/include/webkit-1.0 -Ic:/Dev/MinGw/include/libsoup-2.4\
-Ic:/Dev/MinGw/include/gdk -Ic:/Dev/MinGw/include/glib-2.0 \
-Ic:/Dev/MinGw/lib/glib-2.0/include -Ic:/Dev/MinGw/include/pango-1.0 \
-Ic:/Dev/MinGw/lib/gtk-2.0/include -Ic:/Dev/MinGw/include/atk-1.0 \
-Ic:/Dev/MinGw/include/gtksourceview-2.0 -Ic:/Dev/MinGw/include/loudmouth-1.0 -I../../include \
-Ic:/Dev/MinGw/include/libxml2 \
-Ic:/Dev/MinGw/../Python33/include \
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
