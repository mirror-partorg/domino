CC = gcc
LINK= g++
TMP_DIR=c:/var/work/var/MINGW64_NT-10.0-17134/i686
MAKE=make
AM_MAKEFLAGS=CC=gcc
PYTHON_VERSION_3=3.4
AM_FLAGS=-m32 -DDOMINO78 \
-I../../include \
-I../../var/MINGW64_NT-10.0-17134/x86_64 \
-IC:/Dev/mingw/i686-8.1.0-posix-dwarf-rt_v6-rev0/mingw32/i686-w64-mingw32/include/gtk-3.0 \
-IC:/Dev/mingw/i686-8.1.0-posix-dwarf-rt_v6-rev0/mingw32/i686-w64-mingw32/include/glib-2.0 \
-IC:/Dev/mingw/i686-8.1.0-posix-dwarf-rt_v6-rev0/mingw32/i686-w64-mingw32/lib/glib-2.0/include \
-IC:/Dev/mingw/i686-8.1.0-posix-dwarf-rt_v6-rev0/mingw32/i686-w64-mingw32/include/pango-1.0 \
-IC:/Dev/mingw/i686-8.1.0-posix-dwarf-rt_v6-rev0/mingw32/i686-w64-mingw32/include/cairo \
-IC:/Dev/mingw/i686-8.1.0-posix-dwarf-rt_v6-rev0/mingw32/i686-w64-mingw32/include/gdk-pixbuf-2.0 \
-IC:/Dev/mingw/i686-8.1.0-posix-dwarf-rt_v6-rev0/mingw32/i686-w64-mingw32/include/atk-1.0 \
-IC:/Dev/mingw/i686-8.1.0-posix-dwarf-rt_v6-rev0/mingw32/i686-w64-mingw32/include/webkitgtk-3.0 \
-IC:/Dev/mingw/i686-8.1.0-posix-dwarf-rt_v6-rev0/mingw32/i686-w64-mingw32/include/libsoup-2.4 \
-IC:/Dev/mingw/i686-8.1.0-posix-dwarf-rt_v6-rev0/mingw32/i686-w64-mingw32/include/librsvg-2.0 \
-IC:/Dev/mingw/i686-8.1.0-posix-dwarf-rt_v6-rev0/mingw32/i686-w64-mingw32/lib/gcc/i686-w64-mingw32/8.1.0/include \
-IC:/Python34/include \
-D_WIN32 -DBTI_WIN_32  \
-LC:/Dev/mingw/i686-8.1.0-posix-dwarf-rt_v6-rev0/mingw32/i686-w64-mingw32/lib \
-LC:/Dev/mingw/i686-8.1.0-posix-dwarf-rt_v6-rev0/mingw32/i686-w64-mingw32/lib/gcc/i686-w64-mingw32/8.1.0 \
-IC:/MinGW/mingw32/include \
-IC:/MinGW/mingw32/include/json-c \
-LC:/var/work/src/domino/var/MINGW64_NT-10.0-17134/i686/.libs \
-DZLIB \
-DBINARY_UPDATE 
OPT_FLAGS=-s -g0 -O2
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
LIBWEBKIT=-lwebkitgtk-3.0 -lsoup-2.4 -llibjavascriptcoregtk-3.0
LIBGTK=-lgtk-3 -lgobject-2.0 -lglib-2.0 -lgdk-3 -lpango-1.0 -lgthread-2.0 -lgdk_pixbuf-2.0 -lgio-2.0 -lrsvg-2 
LIBGLIB=-lglib-2.0
LIBLOUDMOUTH=-lloudmouth-1
WINAPP=-mms-bitfields -Wl,-subsystem,windows
WINMM=-lwinmm
misc = ../../misc
DOMINO_BIN=C:/var/work/openSUSE_Leap_15/usr/i686-w64-mingw32/sys-root/mingw/bin
RESOURCES=if [[ "X$(RES)" != "X" ]] ; then C:/Dev/mingw/i686-8.1.0-posix-dwarf-rt_v6-rev0/mingw32/bin/windres.bat -o $(OBJ) $(RES) ; fi