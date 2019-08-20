
include ../../common.mak


LDFLAGS = -lslang -ldomino -ldolib $(LIBGLIB)
CFLAGS = $(GLIBCFLAGS) -DUSE_UTF81

SRCS = main.c option.c c_curses.c c_text_view.c c_util.c handbook.c
SRCS_1 = autologin.c 

APP = handbook
APP_1 = autologin

include ../../program.mak

