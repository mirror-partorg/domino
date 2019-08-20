#ifndef DO_UTILX_H_INCLUDED
#define DO_UTILX_H_INCLUDED

#include <gtk/gtk.h>
#include <domino.h>
#include "do_password.h"
#include "do_message.h"

G_BEGIN_DECLS

typedef struct {
    gint   code;
    gchar *sign;
    gchar *name;
} DoValutaInfo;

typedef struct {
    struct tm tm;
    double quant;
} realiz_t;
typedef struct {
    struct tm tm;
    char name[150];
    double quant;
    double price;
} purchase_t;
#ifdef WIN_RELOAD_ACCEL
gboolean do_key_press_clipboard_event (GtkWidget *widget, GdkEventKey *event, gpointer user_data);
gboolean do_key_press_accel_event (GtkWidget *widget, GdkEventKey *event, gpointer user_data);
#define CLIPBOARD_RU_WIN_32(a) \
          g_signal_connect (a, "key-press-event", G_CALLBACK (do_key_press_clipboard_event), NULL)
#define ACCEL_RU_WIN_32(a) \
          g_signal_connect (a, "key-press-event", G_CALLBACK (do_key_press_accel_event), NULL)
#else
#define CLIPBOARD_RU_WIN_32(a)
#define ACCEL_RU_WIN_32(a)
#endif
#define do_text_in_list(a,b,...) do_alias_text_in_list(a,b,sizeof(b), __VA_ARGS__)
gboolean do_alias_text_in_list(do_alias_t *alias, void *src, size_t size, ...) DO_GNUC_NULL_TERMINATED;


//void on_tree_view_menu_popup(GtkWidget *my_widget, gpointer object);

gboolean on_tree_view_button_pressed( GtkWidget *      view,
                             GdkEventButton * event,
                             gpointer     object);
gboolean on_tree_view_button_released( GtkWidget *      view,
                              GdkEventButton * event,
                              gpointer    object);
/*!!void view_cursor_popup_position_func (GtkMenu  *menu,
                     gint     *x,
                     gint     *y,
                     gboolean *push_in,
                     gpointer  data);
*/
void gtk_tree_view_row_redraw(GtkTreeView *view, GtkTreePath *path);
void gtk_tree_view_row_cursor_redraw(GtkTreeView *view);
void gtk_tree_view_redraw(GtkTreeView *view);
void gtk_tree_view_column_redraw(GtkTreeView *view, GtkTreeViewColumn *column);
GtkTreePath *gtk_tree_path_new_last(GtkTreeModel *model);

void gtk_font_button_get_font_desc(GtkFontButton *font_button,
                                       gchar **font_family, gchar **font_style, gint *font_size);
void gtk_widget_get_font_desc(GtkWidget *widget, const gchar *font_desc,
                                       gchar **font_family, gchar **font_style, gint *font_size);

//void do_error_dialog_show(GtkWidget *parent, const gchar *markup_format, ...);
//gint do_message_dialog_show(GtkWidget *parent, GtkMessageType type, GtkButtonsType buttons, const gchar *markup_format, ...);
//void do_message_window_show(GtkWidget *parent, GtkMessageType type, GtkButtonsType buttons, const gchar *markup_format, ...);
//GtkWidget *do_message_dialog_create(GtkWidget *parent, GtkMessageType type, GtkButtonsType buttons, const gchar *msg);
//void do_print_dialog_show(GtkWidget *parent);

void do_widget_set_property(GObject *widget, const gchar *name, const gchar *value);
void gtk_dialog_accept(GtkDialog *dialog);

gboolean do_window_configure_event_cb(GtkWidget *widget, GdkEventConfigure *event, gchar *path);

gboolean do_domino_auth(DoPassword *password, gpointer data);

gchar *do_find_barcode_dialog(GtkWidget *parent);

//gchar *search_get_text(const gchar *string, gint length);
gchar *search_get_text(const gchar *string, gint length, gint real_len);
gchar *ru_to_en(const gchar *string, gint length);

gboolean do_print_html(GtkWidget *widget, const gchar *text);
#ifndef MIN_SCREEN
gboolean do_common_edit(GtkWidget *widget);
#endif

gchar *do_product_name_format(const gchar *name);
gchar *do_first_alpha_upper(const gchar *name);
//gchar *do_product_coef_format(double value);
gchar *do_money_format(double value, int abr);
gchar *do_money_string(double value, int mantissa, int abr);
gchar *do_money_buh(double value);
gchar *do_big_int_format(int value);
gchar *do_rest_format(double value);
gchar *do_rest_format_ei(double value, double coef, const char *ei);

gchar *do_date_time_format(time_t t);
gchar *do_percent_format(double value);
const gchar *do_get_valuta_name(gint code);
int  do_get_valuta_code(const char *name);
DoValutaInfo *do_utilx_get_valuta_info(gint code);
GList *do_utilx_get_valuta_info_list();
GList *util_string_to_colon(const gchar *text, gint colwidth);

void do_utf8_log(int level, const gchar *format, ...);


void do_tree_cell_data_func_product_name(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data);
void do_tree_cell_data_func_rest(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data);
void do_tree_cell_data_func_big_int(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data);
void do_tree_cell_data_func_big_int_bold(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data);
void do_tree_cell_data_func_big_int_bold_total(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data);
void do_tree_cell_data_func_big_int_bold_depth1(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data);
void do_tree_cell_data_func_rest_bold(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data);
void do_tree_cell_data_func_money(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data);
void do_tree_cell_data_func_money_no_negavite(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data);
void do_tree_cell_data_func_percent(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data);
void do_tree_cell_data_func_percent_bold(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data);
void do_tree_cell_data_func_percent_bold_total(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data);
void do_tree_cell_data_func_percent_bold_depth1(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data);
void do_tree_cell_data_func_money_bold(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data);
void do_tree_cell_data_func_money_bold_total(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data);
void do_tree_cell_data_func_money_bold_depth1(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data);

void do_tree_cell_data_func_money_bold_no_negative(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data);
void do_tree_cell_data_func_money_no_abr(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data);
void do_tree_cell_data_func_money_with_sign(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data);
void do_tree_cell_data_func_date_time(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data);
void do_tree_cell_data_func_date_time_date_only(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data);
void do_tree_cell_data_func_pixbuf_bool(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data);
void do_tree_cell_data_func_pixbuf_warning(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data);
void do_tree_cell_data_func_pixbuf_connect(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data);

gboolean do_tree_view_search_equal_func_date_time(GtkTreeModel *model,
                                                         gint column,
                                                         const gchar *key,
                                                         GtkTreeIter *iter,
                                                         gpointer search_data);

gchar *do_text_utf8_make_compatibility_cp866(const gchar* text);

gboolean do_text_utf8_find_str(const char *haystack, const char *needle);
gint     do_text_utf8_str_cmp_no_case(const char *str1, const char *str2);
//void do_set_main_thread();
int do_gtk_log(const char *str, int level, void *user_data);
void do_gtk_log_off();
void do_gtk_log_on();
void do_gtk_log_init();
void do_start_long_operation(GtkWidget *widget);
void do_end_long_operation(GtkWidget *widget);

gchar *filter_get_text(const gchar *string);
gchar *filter_get_text1(const gchar *string);


gboolean do_utilx_print_pdf_file(GtkWindow *window, const gchar *uri, gboolean show_dialog, gboolean duplex, gboolean margin);
GtkPageSetup *do_utilx_get_default_page_setup(gboolean margin);
#ifndef CASH_1111111111111
gboolean do_utilx_hardware_keycode_is_keyval(GdkEventKey *event, guint keyval);
#endif

void domino_font_style_apply(const gchar *fontname);

gboolean get_realiz(do_alias_t *alias, product_rec_t *product, const char *unit, realiz_t *month, realiz_t *season);
gboolean get_prealiz(do_alias_t *alias, product_rec_t *product, const char *unit, realiz_t *month, realiz_t *season);
gboolean get_purchase(do_alias_t *alias, product_rec_t *product, const char *unit, purchase_t *last);

const gchar *season_name(int season);

gboolean gtk_tree_view_seach_by_product_name_func(GtkTreeModel *model,
                                                         gint column,
                                                         const gchar *key,
                                                         GtkTreeIter *iter,
                                                         gpointer search_data);

#ifdef USE_IM
int send_im_status(const char *user, const char *domain, const char *password, const char *status);
#endif

#ifdef MINIMAL_WIN
int do_show_html(const gchar *html_string);
#endif

G_END_DECLS

#endif // DO_UTILX_H_INCLUDED
