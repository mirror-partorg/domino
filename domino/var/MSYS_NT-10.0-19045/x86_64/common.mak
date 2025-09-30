CC = gcc.exe
LINK= g++.exe
TMP_DIR=z:/dev/msys64/home/mobile1c/work/var/MSYS_NT-10.0-19045/x86_64
MAKE=make
AM_MAKEFLAGS=CC=gcc
PYTHON_VERSION_3=3.12
AM_FLAGS=-DDOMINO78 \
-DWINSOCK \
-I../../include \
-I../../var/MSYS_NT-10.0-19045/x86_64 \
-IZ:/dev/msys64/usr/include \
-IZ:/dev/msys64/usr/include/gtk-3.0 \
-IZ:/dev/msys64/usr/include/glib-2.0 \
-Iz:/dev/msys64/mingw64/lib/glib-2.0/include \
-IZ:/dev/msys64/usr/include/pango-1.0 \
-IZ:/dev/msys64/usr/include/cairo \
-IZ:/dev/msys64/usr/include/gdk-pixbuf-2.0 \
-IZ:/dev/msys64/usr/include/atk-1.0 \
-IZ:/dev/msys64/usr/include/webkitgtk-3.0 \
-IZ:/dev/msys64/usr/include/libsoup-2.4 \
-IZ:/dev/msys64/usr/include/librsvg-2.0 \
-IZ:/dev/msys64/usr/include/harfbuzz \
-IZ:/dev/msys64/usr/include/python3.12 \
-IZ:/dev/msys64/usr/include/json-glib-1.0 \
-IZ:/dev/msys64/usr/lib/gcc/x86_64-pc-cygwin/15.1.0/include \
-IZ:/dev/msys64/usr/include/w32api \
-IC:/Users/rishes/Local/Programs/Python/Python38/include  \
-D_WIN32 -DBTI_WIN_32  \
-IC:/MinGW/mingw64/include \
-IC:/MinGW/mingw64/include/json-c \
-LZ:/dev/msys64/home/mobile1c/work/var/MINGW64_NT-10.0-19045/x86_64/.libs \
-L${TMP_DIR}/.libs \
-L"C:/PVSW/BIN64" \
-DZLIB \
-DBINARY_UPDATE 
OPT_FLAGS=-g
OPT_FLAGS1=-s -O2
LIBWINSOCK=-lws2_32
LIBICONV=-liconv
LIBZ=-lz
LIBEXPAT=-lexpat
LIBFBCLIENT=-lfbclient_ms
LIBDO=-ldolib
LIBVLC=-lvlc
LIBDOMINO=-ldomino
LIBSOUP=-lsoup-2.4
LIBPSQL=-lw64btrv
LIBJSONGLIB=-ljson-glib-1.0
LIBSQLITE3=-lsqlite3
LIBXML2=-llibxml2
LIBGTK=-lgtk-3 -lgobject-2.0 -lglib-2.0 -lgdk-3 -lpango-1.0 -lgthread-2.0 -lgdk_pixbuf-2.0 -lgio-2.0 -lrsvg-2 
LIBGLIB=-lglib-2.0
LIBLOUDMOUTH=-lloudmouth-1
PYTHON_LIBS_3=-lpython3.12 -LC:/Users/rishes/Local/Programs/Python/Python38
WINAPP=-mms-bitfields -Wl,-subsystem,windows
WINMM=-lwinmm
MAKEDLL=1
misc = ../../misc
DOMINO_BIN=C:/var/work/openSUSE_Leap_15/usr/x86_64-w64-mingw32/sys-root/mingw/bin
RESOURCES=if [[ "X$(RES)" != "X" ]] ; then z:/dev/mingw/x86_64-8.1.0-posix-seh-rt_v6-rev0/mingw64/bin/windres.bat -o $(OBJ) $(RES) ; fi