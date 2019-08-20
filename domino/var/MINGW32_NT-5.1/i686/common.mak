CC = gcc 
TMP_DIR=var/MINGW32_NT-5.1/i686
MAKE=make
AM_MAKEFLAGS=CC=gcc
AM_FLAGS=-LE:/dev/MinGw/../lib/bin -LE:/dev/MinGw/lib  -L../../var/MINGW32_NT-5.1/i686/.libs \
-D_WIN32 -DBTI_WIN_32 -DWINSOCK \
-IE:/dev/MinGw/include/gdk-pixbuf-2.0 \
-IE:/dev/MinGw/include/gtk-2.0 -IE:/dev/MinGw/include/cairo \
-IE:/dev/MinGw/include/webkit-1.0 -IE:/dev/MinGw/include/libsoup-2.4\
-IE:/dev/MinGw/include/gdk -IE:/dev/MinGw/include/glib-2.0 \
-IE:/dev/MinGw/lib/glib-2.0/include -IE:/dev/MinGw/include/pango-1.0 -IE:/dev/MinGw/include/poppler/glib \
-IE:/dev/MinGw/lib/gtk-2.0/include -IE:/dev/MinGw/include/atk-1.0 \
-IE:/dev/MinGw/include/gtksourceview-2.0 -IE:/dev/MinGw/include/loudmouth-1.0 -I../../include \
-IE:/dev/MinGw/include/libxml2 \
-IT:/domino/var/MINGW32_NT-5.1/i686 \
-DARTIX \
-DEXPAT \
-DZLIB \
-mms-bitfields -Wl,-subsystem,windows \
-DWIN_RELOAD_ACCEL \
-DUSE_LOUDMOUTH \
-s -O2
LIBWINSOCK=-lws2_32
LIBICONV=-liconv
LIBZ=-lz
LIBEXPAT=-lexpat
LIBFBCLIENT=-lfbclient_ms
PYTHON_VERSION_3=3.3
PYTHON_CFLAGS_3=-IE:/dev/MinGw/include/python33
PYTHON_LIBS_3=-lpython33
LIBDO=-ldolib
LIBVLC=-lvlc
LIBDOMINO=-ldomino
LIBARTIX=-lartix
LIBSOUP=-lsoup-2.4
LIBPSQL=-lw3btrv7
LIBXML2=-llibxml2
LIBEXPAT=-lexpat
LIBWEBKIT=-lwebkitgtk-1.0
LIBGTK=-lgtk-win32-2.0 -lgobject-2.0 -lglib-2.0 -lgdk-win32-2.0 -lpango-1.0 -lgthread-2.0 -lgdk_pixbuf-2.0 -lgio-2.0
LIBGLIB=-lglib-2.0
LIBLOUDMOUTH=-lloudmouth-1
WINAPP=-mms-bitfields -Wl,-subsystem,windows
LIBPOPPLER=-lpoppler-glib -lcairo
WINMM=-lwinmm
misc = ../../misc
