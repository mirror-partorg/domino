#ifndef DO_UTILX_H_INCLUDED
#define DO_UTILX_H_INCLUDED

#include <gtk/gtk.h>

G_BEGIN_DECLS

const char *domino_unit();

gboolean on_tree_view_button_pressed( GtkWidget *      view,
                             GdkEventButton * event,
                             gpointer     object);
gboolean on_tree_view_button_released( GtkWidget *      view,
                              GdkEventButton * event,
                              gpointer    object);
void gtk_tree_view_row_redraw(GtkTreeView *view, GtkTreePath *path);
void gtk_tree_view_row_cursor_redraw(GtkTreeView *view);
void gtk_tree_view_redraw(GtkTreeView *view);
void gtk_tree_view_column_redraw(GtkTreeView *view, GtkTreeViewColumn *column);
GtkTreePath *gtk_tree_path_new_last(GtkTreeModel *model);

void gtk_font_button_get_font_desc(GtkFontButton *font_button,
                                       gchar **font_family, gchar **font_style, gint *font_size);
void gtk_widget_get_font_desc(GtkWidget *widget, const gchar *font_desc,
                                       gchar **font_family, gchar **font_style, gint *font_size);

void do_widget_set_property(GObject *widget, const gchar *name, const gchar *value);
void gtk_dialog_accept(GtkDialog *dialog);

gchar *search_get_text(const gchar *string, gint length, gint real_len);
gchar *ru_to_en(const gchar *string, gint length);

gboolean do_print_html(GtkWidget *widget, const gchar *text);
gboolean do_common_edit(GtkWidget *widget);

gchar *do_product_name_format(const gchar *name);
gchar *do_first_alpha_upper(const gchar *name);

gchar *do_money_format(double value, int abr);
gchar *do_money_string(double value, int mantissa, int abr);
gchar *do_money_buh(double value);
gchar *do_big_int_format(int value);
gchar *do_rest_format(double value);
gchar *do_rest_format_ei(double value, double coef, const char *ei);

gchar *do_date_time_format(time_t t);
gchar *do_percent_format(double value);
GList *util_string_to_colon(const gchar *text, gint colwidth);
gchar *markup_plus(const gchar *markup);

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
gboolean do_utilx_hardware_keycode_is_keyval(GdkEventKey *event, guint keyval);
void domino_font_style_apply(const gchar *fontname);
const gchar *season_name(int season);
gboolean gtk_tree_view_seach_by_product_name_func(GtkTreeModel *model,
                                                         gint column,
                                                         const gchar *key,
                                                         GtkTreeIter *iter,
                                                         gpointer search_data);

#ifdef MINIMAL_WIN
int do_show_html(const gchar *html_string);
#endif

G_END_DECLS

#endif // DO_UTILX_H_INCLUDED
