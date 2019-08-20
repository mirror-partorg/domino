#ifndef c_TEXT_VIEW_H_INCLUDED
#define c_TEXT_VIEW_H_INCLUDED

#include <domino.h>
#include "c_curses.h"

typedef struct _c_text_view_t c_text_view_t;
typedef int  (*c_text_view_filter_changed_func) (const char *filter, void *user_data);
typedef int  (*c_text_view_mode_changed_func) (int mode, void *user_data);
typedef int  (*c_text_view_row_activated_func) (int index, void *user_data);
typedef int  (*c_text_view_row_selected_func) (int index, int unselect, void *user_data);
typedef do_string_list_t *(*c_text_view_list_ending_func) (void *user_data);
typedef char *(*c_text_view_row_text_func) (int index, const char *text, int max_text_len, void *user_data);


#ifdef __cplusplus
extern "C"
{
#endif

c_text_view_t *c_text_view_new(int escaped, do_string_list_t *footer_str);
void           c_text_view_run(c_text_view_t *view);
void           c_text_view_set_filter_changed_func(c_text_view_t *view, c_text_view_filter_changed_func func, void *user_data);
void           c_text_view_set_mode_changed_func(c_text_view_t *view, c_text_view_mode_changed_func func, void *user_data);
void           c_text_view_set_row_activated_func(c_text_view_t *view, c_text_view_row_activated_func func, void *user_data);
void           c_text_view_set_row_selected_func(c_text_view_t *view, c_text_view_row_selected_func func, void *user_data);
void           c_text_view_set_row_text_func(c_text_view_t *view, c_text_view_row_text_func func, void *user_data);
void           c_text_view_set_list_ending_func(c_text_view_t *view, c_text_view_list_ending_func func, void *user_data);
void           c_text_view_set_list(c_text_view_t *view, do_string_list_t *list);
void           c_text_view_free(c_text_view_t *view);
void           c_text_view_resize(c_text_view_t *view);
void           c_text_view_set_sel(c_text_view_t *view, int indx);
void           c_text_view_refresh_footer(c_text_view_t *view);

#ifdef __cplusplus
}
#endif


#endif // c_TEXT_VIEW_H_INCLUDED
