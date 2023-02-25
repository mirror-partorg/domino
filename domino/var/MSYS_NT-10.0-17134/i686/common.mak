CC = C:/domino/msys64/mingw32/bin/gcc.bat 
LINK= C:/domino/msys64/mingw32/bin/g++.bat
TMP_DIR=c:/var/work/var/MSYS_NT-10.0-17134/i686
MAKE=make
AM_MAKEFLAGS=CC=gcc
PYTHON_VERSION_3=3.4
AM_FLAGS=-m32 -DDOMINO78 \
-I../../include \
-I../../var/MSYS_NT-10.0-17134/x86_64 \
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
-LC:/var/work/var/MSYS_NT-10.0-17134/i686/.libs \
-L"C:/Program Files (x86)/Actian/Zen/bin" \
-DZLIB \
-DWINSOCK \
-DBINARY_UPDATE 
OPT_FLAGS=-s -g0 -O2
LIBICONV=-liconv
LIBZ=-lz
LIBEXPAT=-lexpat
LIBWINSOCK=-lws2_32
LIBFBCLIENT=-lfbclient_ms
LIBDO=-ldolib
LIBVLC=-lvlc
LIBDOMINO=-ldomino
LIBSOUP=-lsoup-2.4
LIBPSQL=-lw3btrv7
LIBXML2=-llibxml2
LIBWEBKIT=-lwebkitgtk-3.0 -lsoup-2.4 -llibjavascriptcoregtk-3.0
PYTHON_LIBS_3=-lpython34 -LC:/domino/bin/Python34
LIBGTK=-lgtk-3 -lgobject-2.0 -lglib-2.0 -lgdk-3 -lpango-1.0 -lgthread-2.0 -lgdk_pixbuf-2.0 -lgio-2.0 -lrsvg-2 
LIBGLIB=-lglib-2.0
LIBLOUDMOUTH=-lloudmouth-1
WINAPP=-mms-bitfields -Wl,-subsystem,windows
WINMM=-lwinmm
MAKEDLL=1
misc = ../../misc
DOMINO_BIN=C:/var/work/openSUSE_Leap_15/usr/i686-w64-mingw32/sys-root/mingw/bin
RESOURCES=if [[ "X$(RES)" != "X" ]] ; then C:/Dev/mingw/i686-8.1.0-posix-dwarf-rt_v6-rev0/mingw32/bin/windres.bat -o $(OBJ) $(RES) ; fi