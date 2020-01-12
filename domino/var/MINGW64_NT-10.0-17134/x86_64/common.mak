CC = gcc.exe
LINK= g++.exe
TMP_DIR=C:/var/work/var/MINGW64_NT-10.0-17134/x86_64
MAKE=make
AM_MAKEFLAGS=CC=gcc
PYTHON_VERSION_3=3.8
AM_FLAGS=-DDOMINO78 \
-DWINSOCK \
-I../../include \
-I../../var/MINGW64_NT-10.0-17134/x86_64 \
-IC:/Users/rishes/Local/Programs/Python/Python38/include  \
-D_WIN32 -DBTI_WIN_32  \
-L${TMP_DIR}/.libs \
-L"C:\Program Files\Actian\Zen\bin"  \
-DZLIB \
-DBINARY_UPDATE 
OPT_FLAGS=-s -O2
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
LIBWEBKIT=-lwebkitgtk-3.0 -lsoup-2.4 -llibjavascriptcoregtk-3.0
LIBGTK=-lgtk-3 -lgobject-2.0 -lglib-2.0 -lgdk-3 -lpango-1.0 -lgthread-2.0 -lgdk_pixbuf-2.0 -lgio-2.0 -lrsvg-2 
LIBGLIB=-lglib-2.0
LIBLOUDMOUTH=-lloudmouth-1
PYTHON_LIBS_3=-lpython38 -LC:/Users/rishes/Local/Programs/Python/Python38
WINAPP=-mms-bitfields -Wl,-subsystem,windows
WINMM=-lwinmm
MAKEDLL=1
misc = ../../misc
DOMINO_BIN=C:/var/work/openSUSE_Leap_15/usr/x86_64-w64-mingw32/sys-root/mingw/bin
RESOURCES=if [[ "X$(RES)" != "X" ]] ; then C:/Dev/mingw/x86_64-8.1.0-posix-seh-rt_v6-rev0/mingw64/bin/windres.bat -o $(OBJ) $(RES) ; fi