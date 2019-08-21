include ../../common.mak

lib_dox  = ../domino/

LDFLAGS = $(LIBGTK) $(WINAPP_) $(LIBDO) $(LIBWEBKIT) $(LIBEXPAT) -Wfatal-errors

CFLAGS = -DUSE_GTK -DUSE_UTF8 $(GTKCFLAGS) $(WEBKITCFLAG) -DNODOMINO -DEXPAT -Wfatal-errors -DWEBKIT2


SRCS = main.c \
		do_ads_view.c \
		do_application.c \
		do_common_actions.c \
		do_config.c \
		do_context.c \
		do_entry.c \
		do_filter.c \
		do_marshal.c \
		do_message.c \
		do_notebook.c \
		do_setting_view.c \
		do_tree_view.c \
		do_utilx.c \
		do_validate.c \
		do_view.c \
		do_view_actions.c \
		do_window.c \
		hig.c \
		domino.c 

APP = bookcase
OBJ = bookcase.o
RES = bookcase.rc

include ../../program.mak
