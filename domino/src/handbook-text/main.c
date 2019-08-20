#include "curses.h"
#include <domino.h>
#include "option.h"
#include "../config/config.h"
#include "version.h"
#include "handbook.h"
#include "c_text_view.h"
#include <string.h>
#include <locale.h>
#include <unistd.h>
#include <signal.h>

#include "../misc/iconv_.h"

static int filter_changed(const char *filter, void *user_data);
static handbook_t *handbook;
static c_text_view_t *view;
static int row_activated(int index, void *user_data);
static option_t *opt = NULL;
const char *exchanged_name = "Handbook";
iconv_t conv;

static char *row_text(int index, const char *text, int max_text_len, void *user_data);
static int init()
{
    char *str;
    setlocale(LC_ALL, "");
    str = get_locale_lang();
    conv = iconv_open(str, "UTF-8");
    if (!option_read_config(opt))
        exit(1);
    do_log_set_type(exchanged_name, opt->log_type, opt->log_file);
    if  (!domino_init(opt->domino_config_file, FALSE, DO_PARAM_END))
        exit(1);

    c_curses_init();
    return 1;
}

int main(int argc, char *argv[])
{
    do_log_set_type("handbook", DO_LOG_TYPE_SYSLOG, NULL);
    opt = option_new();
    if (option_parse_options(opt, argc, argv))
       exit(1);
    if (opt->show_version) {
        printf("handbook " HANDBOOK_TEXT_RELEASE " (" ADATE ") \n");
        exit(0);
    }
    init();

    do_log_set_type("handbook", DO_LOG_TYPE_SYSLOG, NULL);
    handbook = handbook_new(opt->artix_host, opt->artix_cs, opt->artix_user, opt->artix_password);
    if (!handbook)
        exit(1);
    char *footer_str=iconv_(conv, "Итог,Заказ   ,Стрн,Отмена  ,Рег.,МНН/Торг.");
    view = c_text_view_new(0, do_string_list_new_from_csv(footer_str));
    do_free(footer_str);
    handbook_set_marked_name(handbook, DOMINO_MARKED, iconv_(conv, "Заявка"));
    handbook_set_marked_name(handbook, DOMINO_MARKED_AND_ACCEPT, iconv_(conv, "Заказ"));
    handbook_set_marked_name(handbook, DOMINO_MARKED_AND_REJECT, iconv_(conv, "Отказ"));
    c_text_view_set_filter_changed_func(view, filter_changed, NULL);
    c_text_view_set_row_activated_func(view, row_activated, NULL);
    c_text_view_set_row_selected_func(view, (c_text_view_row_selected_func)handbook_row_seleted, handbook);
    c_text_view_set_row_text_func(view, row_text, NULL);
    c_text_view_set_list_ending_func(view, (c_text_view_list_ending_func)handbook_get_continue, handbook);
    c_text_view_set_mode_changed_func(view, (c_text_view_mode_changed_func)handbook_mode_changed, handbook);
    //c_text_view_set_list(view, handbook_get_product_list(handbook, "оп"));

    c_text_view_run(view);
    c_text_view_free(view);

    c_curses_stop();
    handbook_free(handbook);
    return 0;
}
static int filter_changed(const char *filter, void *user_data)
{
    c_text_view_set_list(view, handbook_get_product_list(handbook, filter));
    return 1;
}
static int row_activated(int index, void *user_data)
{
    c_text_view_t *v;
    do_string_list_t *info;
    int i = 0, clickable;
    char *footer;

    info = handbook_get_product_info(handbook, index, c_curses_get_col_n(), &i, &clickable, &footer);
    if ( !info )
        return 0;
    v = c_text_view_new(1, (footer) ? do_string_list_new_from_csv(footer) : NULL);
    if ( footer )
	do_free(footer);
    c_text_view_set_list(v, info);
    c_text_view_set_sel(v, i);
    do_string_list_free(info);
    if ( clickable ) {
        //c_text_view_set_filter_changed_func(v, filter_changed, NULL);
        c_text_view_set_row_activated_func(v, row_activated, NULL);
        c_text_view_set_row_selected_func(v, (c_text_view_row_selected_func)handbook_row_seleted, handbook);
        c_text_view_set_row_text_func(v, row_text, NULL);
        c_text_view_set_list_ending_func(v, (c_text_view_list_ending_func)handbook_get_continue, handbook);
        c_text_view_set_mode_changed_func(v, (c_text_view_mode_changed_func)handbook_mode_changed, handbook);
    }
    c_text_view_run(v);
    c_text_view_free(v);
    c_text_view_refresh_footer(view);
    if ( clickable ) {
        handbook_stack_pop(handbook);
    }
    return 1;
}
static char *row_text(int index, const char *text, int max_text_len, void *user_data)
{
    return handbook_get_product_row(handbook, index, text, max_text_len);
}



