#ifndef CURSES_META_H_INCLUDED
#define CURSES_META_H_INCLUDED

#include <slcurses.h>
#define curses_window_t SLcurses_Window_Type
#define KEY_ESC 27

#include "c_text_view.h"
#include "c_util.h"

#define C_COLOR_DEFAULT 1
#define C_COLOR_SELECT 2
#define C_COLOR_BLACK 3


#ifdef __cplusplus
extern "C"
{
#endif

void c_curses_init();
void c_curses_stop();
int  c_curses_wgetch(curses_window_t *win);
void c_curses_insert_widget(void *widget);
void c_curses_delete_widget(void *widget);
int  c_curses_get_col_n();

#ifdef __cplusplus
}
#endif


#endif
