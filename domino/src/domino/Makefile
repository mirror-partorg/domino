include ../../common.mak

lib_dox   = .


LDFLAGS =  $(LIBDO) $(LIBCUPS) $(LIBLOUDMOUTH) $(LIBARTIX) $(LIBDOMINO) $(LIBWEBKIT) $(LIBGTK) $(LIBWINSOCK) $(LIBICONV)  $(WINAPP) $(WINMM) $(LIBSOUP) $(LIBEV) $(LIBPOPPLER)

CFLAGS = -D_REENTRANT -DUSE_GTK  -DUSE_UTF8 -DUSE_DEPRECATE_PRINT $(ARTIXCFLAG) $(GTKCFLAGS) $(WEBKITCFLAG) -DSHOW_ADS1 $(EVCFLAGS) $(POPPLERCFLAGS)  -DADS_RECOMMEND1 -DSHOW_CERT -DSMALLSTOCK_COL


SRCS = main.c \
        $(lib_dox)/do_about_dialog.c  \
        $(lib_dox)/do_common_actions.c $(lib_dox)/do_context.c $(lib_dox)/do_enum_types.c $(lib_dox)/do_html_view.c \
        $(lib_dox)/do_icons.c $(lib_dox)/do_keyboard.c $(lib_dox)/do_keyboard_dialog.c $(lib_dox)/do_keyboard_model.c \
        $(lib_dox)/do_keyboard_view.c $(lib_dox)/do_marshal.c $(lib_dox)/do_notebook.c $(lib_dox)/do_password.c \
        $(lib_dox)/do_password_dialog.c $(lib_dox)/do_product_model.c  $(lib_dox)/do_product_view.c \
        $(lib_dox)/do_tabs_menu.c $(lib_dox)/do_utilx.c $(lib_dox)/do_message.c\
        $(lib_dox)/do_view.c $(lib_dox)/do_view_actions.c $(lib_dox)/do_window.c \
        $(lib_dox)/do_order_view.c $(lib_dox)/do_limit_view.c $(lib_dox)/do_unit_box.c  \
        $(lib_dox)/domino.c $(lib_dox)/do_setting_view.c $(lib_dox)/do_cash_view.c \
        $(lib_dox)/do_pko_dialog.c \
	$(lib_dox)/do_product_edit.c \
	$(lib_dox)/do_entry.c $(lib_dox)/do_entry_box.c $(lib_dox)/do_entry_calc.c $(lib_dox)/do_entry_date.c \
	$(lib_dox)/do_barcode_dialog.c $(lib_dox)/do_product_embed_edit.c $(lib_dox)/do_check_dialog.c $(lib_dox)/do_filter.c \
	$(lib_dox)/do_entry_date_box.c $(lib_dox)/do_validate.c $(lib_dox)/do_tree_view.c \
	$(lib_dox)/do_rz_view.c $(lib_dox)/do_rz_edit.c $(lib_dox)/do_rz_edit_view.c $(lib_dox)/do_input_csv.c \
	$(lib_dox)/do_rz_select.c $(lib_dox)/do_search.c \
	$(lib_dox)/hig.c $(lib_dox)/do_discount_dialog.c $(lib_dox)/do_discount_view.c\
	$(lib_dox)/do_enum_value.c $(lib_dox)/do_enum_box.c $(lib_dox)/do_report_view.c $(lib_dox)/do_report_unit_model.c \
	$(lib_dox)/do_entry_partner.c $(lib_dox)/do_entry_partner_box.c \
	$(lib_dox)/do_seller_dialog.c $(lib_dox)/do_print.c $(lib_dox)/do_schedule_dialog.c $(lib_dox)/do_schedule_view.c \
	$(lib_dox)/do_sale_dialog.c $(lib_dox)/do_sale_view.c $(lib_dox)/do_purchase_dialog.c $(lib_dox)/do_purchase_view.c \
        $(lib_dox)/do_unit_period_dialog.c $(lib_dox)/do_rest_view.c \
	$(misc)/print.c $(misc)/simple_expr.c $(misc)/mthrift.c $(misc)/iconv_.c  \
	$(misc)/template.c $(misc)/utf8util.c  $(misc)/cash_key_command.c

APP = domino
OBJ = domino.o

INSTALL=cp ../../template/domino.desktop /usr/share/applications/domino.desktop && \
        find ../../res/icons -name bookcase.png | awk '{split($$0,a,"/"); print " /usr/share/icons/hicolor/" a[5] "/apps/";}' | xargs -l mkdir -p && \
        find ../../res/icons -name bookcase.png | awk '{split($$0,a,"/"); print $$0 " /usr/share/icons/hicolor/" a[5] "/apps/" a[6];}' | xargs -l cp -f && \
        gtk-update-icon-cache -f /usr/share/icons/hicolor

include ../../program.mak
