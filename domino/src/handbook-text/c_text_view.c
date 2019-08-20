#include "c_text_view.h"
#include <domino.h>


int refresh_box = FALSE;
int refresh_kb = TRUE;

struct _c_text_view_t {
    do_string_list_t *list;
    int top_line;
    int sel_line;
    curses_window_t *win;
    curses_window_t *footer;
    int n_row;
    int n_col;
    int escaped;
    char filter_text[1024];
    int running;
    do_string_list_t *footer_lst;

    c_text_view_filter_changed_func filter_changed_func;
    void *filter_changed_user_data;
    c_text_view_mode_changed_func mode_changed_func;
    void *mode_changed_user_data;
    c_text_view_row_activated_func row_activated_func;
    void *row_activated_user_data;
    c_text_view_row_selected_func row_selected_func;
    void *row_selected_user_data;
    c_text_view_row_text_func row_text_func;
    void *row_text_user_data;
    c_text_view_list_ending_func list_ending_func;
    void *list_ending_user_data;
    int mode;

};

static void c_text_view_refresh(c_text_view_t *view);
static int c_text_view_check_movement_keys(int key, c_text_view_t *view);
void c_text_view_refresh_footer(c_text_view_t *view);

void c_text_view_set_sel(c_text_view_t *view, int indx)
{
    if (view->list->count <= indx)
        return;
    view->sel_line = indx - view->top_line;

    if (view->sel_line >= view->n_row) {
        view->top_line += view->sel_line - view->n_row;
        view->sel_line = view->n_row - 1;
    }
    if (view->sel_line > view->list->count - 1 - view->top_line)
        view->sel_line = view->list->count - 1 - view->top_line;

    c_text_view_refresh(view);
}

#define colored_ctype(ch, attr, pair) \
        ((ch) | (attr) | COLOR_PAIR(pair))

static void c_text_view_init_color(c_text_view_t *view)
{
    SLtt_set_color (0, (char *) "", "color7", "brightblue");
    SLtt_set_color (1, (char *) "", "color7", "brightblue");
    SLtt_set_color (2, (char *) "", "black", "color6");
    SLtt_set_color (3, (char *) "", "white", "black");

}
c_text_view_t *c_text_view_new(int escaped, do_string_list_t *footer_str)
{
    c_text_view_t *retval;

    retval = do_malloc0(sizeof(c_text_view_t));
    retval->win = newwin(LINES-((footer_str) ? 1 : 0), COLS, 0, 0);

    if ( footer_str ) {
        retval->footer_lst = footer_str;
        retval->footer = newwin(1, COLS, LINES-1, 0);
    }

    //retval->win->_begy = -1;//!!
    //retval->win->_begx = -1;

    retval->escaped = escaped;
    retval->n_row = retval->win->_maxy - retval->win->_begy - 1;
    retval->n_col = retval->win->_maxx - retval->win->_begx - 1;
    retval->list = do_string_list_new();
    //init_pair(1,COLOR_WHITE,COLOR_BLUE);
    c_text_view_init_color(retval);
    //SLtt_set_color (3, (char *) "", "white", "brightblue");
    //SLtt_set_color (4, (char *) "", "white", "brightblue");

    wattrset(retval->win,COLOR_PAIR(1));
    retval->win->has_box = 1;
    retval->win->modified = 1;
    /*if ( refresh_kb )
        box(retval->win, '|','-');
    else
        box(retval->win, 0, 0);*/

    keypad(retval->win, TRUE);
    c_curses_insert_widget(retval);
    c_text_view_refresh_footer(retval);


    //retval->mode=1;
    return retval;
}
void c_text_view_refresh_footer(c_text_view_t *view)
{
    if ( !view->footer )
        return;

    wattrset(view->footer,COLOR_PAIR(2));
    //view->footer->has_box = 1;
    view->footer->modified = 1;

    mvwprintw(view->footer, 0, 0, "%*s", view->n_col + ((view->win->has_box) ? 2 : 0), "");
    int i,pos;
    for ( i = 0,pos=0; i < view->footer_lst->count; pos+=c_strlen(view->footer_lst->list[i++]) ) {
        if ( !(i%2) )
            wattron(view->footer, COLOR_PAIR(C_COLOR_BLACK));
        else
            wattron(view->footer, COLOR_PAIR(C_COLOR_SELECT));
        mvwprintw(view->footer, 0, pos, "%s",view->footer_lst->list[i]);
    }
    wattron(view->footer, COLOR_PAIR(C_COLOR_DEFAULT));

    wnoutrefresh(view->footer);

}

void c_text_view_resize(c_text_view_t *view)
{

    delwin(view->win);
    c_text_view_init_color(view);

    view->win = SLcurses_newwin(LINES, COLS, 0, 0);
    //view->win->_begy = -1;
    //view->win->_begx = -1;

    //!!view->win->has_box = 1;
    view->win->modified = 1;
    view->win->color = 1;

    view->n_row = view->win->_maxy - view->win->_begy - 1;
    view->n_col = view->win->_maxx - view->win->_begx - 1;
    if (view->filter_changed_func)
        view->n_row--;
    //box(view->win, 0, 0);
    keypad(view->win, TRUE);
    if (view->sel_line > view->n_row - 1)
        view->sel_line = view->n_row - 1;
    c_text_view_refresh(view);
}

void c_text_view_set_filter_changed_func(c_text_view_t *view, c_text_view_filter_changed_func func, void *user_data)
{
    view->filter_changed_func = func;
    view->filter_changed_user_data = user_data;
    view->n_row = view->win->_maxy - view->win->_begy - 1;
    if (view->filter_changed_func)
        view->n_row--;
}
void c_text_view_set_mode_changed_func(c_text_view_t *view, c_text_view_mode_changed_func func, void *user_data)
{
    view->mode_changed_func = func;
    view->mode_changed_user_data = user_data;
    view->n_row = view->win->_maxy - view->win->_begy - 1;
    if (view->mode_changed_func)
        view->n_row--;
}
void c_text_view_set_list_ending_func(c_text_view_t *view, c_text_view_list_ending_func func, void *user_data)
{
    view->list_ending_func = func;
    view->list_ending_user_data = user_data;
}

void c_text_view_set_row_selected_func(c_text_view_t *view, c_text_view_row_selected_func func, void *user_data)
{
    view->row_selected_func = func;
    view->row_selected_user_data = user_data;
}
void c_text_view_set_row_activated_func(c_text_view_t *view, c_text_view_row_activated_func func, void *user_data)
{
    view->row_activated_func = func;
    view->row_activated_user_data = user_data;
}
void c_text_view_set_row_text_func(c_text_view_t *view, c_text_view_row_text_func func, void *user_data)
{
    view->row_text_func = func;
    view->row_text_user_data = user_data;
}
void c_text_view_set_list(c_text_view_t *view, do_string_list_t *list)
{
    if ( view->list )
        do_string_list_free(view->list);
    view->top_line = 0;
    view->sel_line = 0;
    if ( list ) {
        view->list = do_string_list_dup(list);
    }
    else
        view->list = do_string_list_new();
    c_text_view_refresh(view);
}
void c_text_view_free(c_text_view_t *view)
{
    c_curses_delete_widget(view);
    keypad(view->win, FALSE);
    delwin(view->win);
    if ( view->footer )
        delwin(view->footer);
    if ( view->footer_lst )
        do_string_list_free(view->footer_lst);
    do_free(view);
}
void c_text_view_run(c_text_view_t *view)
{
    int ch, alpha;
    int len;
    char *p;
    view->running = 1;
    if (view->filter_changed_func)
        view->filter_changed_func("", view->filter_changed_user_data);
    c_text_view_refresh(view);
    while ((ch = c_curses_wgetch(view->win)) != KEY_F(10)) {
        if ( refresh_kb ) {
            refresh_box = TRUE;
            c_text_view_refresh(view);
            refresh_kb = FALSE;
        }
        if ( ch == KEY_F(12) || ch == KEY_F(2)  ) {
            if ( view->row_selected_func ) {
                if ( view->row_selected_func(view->sel_line + view->top_line,
                     ch == KEY_F(2), view->row_selected_user_data) )
                    c_text_view_refresh(view);
                }
            continue;
        }
        if ( ch == KEY_F(7) ) {
            if ( view->mode_changed_func ) {
                view->mode = !view->mode;
                (view->mode_changed_func)(view->mode, view->mode_changed_user_data);
                if ( view->filter_changed_func )
                    view->filter_changed_func("", view->filter_changed_user_data);
                else
                    c_text_view_set_list(view, NULL);
                c_text_view_refresh(view);
                if (view->filter_changed_func && strlen(view->filter_text) ) {
                    view->filter_changed_func(view->filter_text, view->filter_changed_user_data);
                    c_text_view_refresh(view);
                }
            }

        }
        if (ch == KEY_ESC && view->escaped)
            break;
        if (c_text_view_check_movement_keys(ch, view))
            continue;
        if ( c_isenter(ch) ) {
            if (view->list->count) {
                if (view->row_activated_func)
                    (view->row_activated_func)(view->sel_line + view->top_line, view->row_activated_user_data);
                c_text_view_refresh(view);
            }
            continue;
        }
        if (view->filter_changed_func) {
            len = -1;
            if ((alpha = c_upper_ru(ch)) != 0) {
                len = strlen(view->filter_text) + c_chlen(alpha);
                strncpy(view->filter_text + strlen(view->filter_text), (char*)&alpha, c_chlen(alpha));
                view->filter_text[len] = '\0';
            }
            else
            if (c_isbackspace(ch)) {
                if (strlen(view->filter_text) > 0) {
                    p = c_str_at(view->filter_text, c_strlen(view->filter_text) - 1);
                    *p = '\0';
                    len = 0;
                }
            }
            else
//            if (ch == SL_KEY_DELETE || ch == KEY_ESC) {
            if (ch == KEY_ESC) {
                if (strlen(view->filter_text) > 0) {
                    *view->filter_text = '\0';
                    len = 0;
                }
            }
            if (len != -1) {
                view->filter_changed_func(view->filter_text, view->filter_changed_user_data);
                c_text_view_refresh(view);
                continue;
            }
        }
        //!! mvwprintw(view->win, 1, 1, "%d  ", ch);
    }
    view->running = 0;
}
static void c_text_view_refresh(c_text_view_t *view)
{
    if (!view->running)
        return;

    int i;
    char *text, *buf;
    char frm[40];
    const char *empty = "";
    int top = 1;
    if (view->filter_changed_func)
        top++;

    wnoutrefresh(view->win);
    wattron(view->win, COLOR_PAIR(C_COLOR_DEFAULT));
    if ( refresh_box ) {
        //!!box(view->win, 0, 0);
        clearok(view->win, 1);
        refresh_box = FALSE;
    }
    if (view->filter_changed_func) {
        if ( view->mode == 1 ) {
            sprintf(frm, "MHH: %%%ds", (int)(view->n_col + (strlen(view->filter_text) - c_strlen(view->filter_text)))-5);
            mvwprintw(view->win, 1, 1, frm, view->filter_text, " ");
        }
        else {
            sprintf(frm, "%%%ds", (int)(view->n_col + (strlen(view->filter_text) - c_strlen(view->filter_text))));
            mvwprintw(view->win, 1, 1, frm, view->filter_text);
        }
    }
    for (i = 0; i < view->n_row; i++) {
        if (i + view->top_line < view->list->count)//!!
            if (view->row_text_func)
                text = view->row_text_func(i + view->top_line, view->list->list[i + view->top_line], view->n_col, view->row_text_user_data);
            else
                text = do_strdup(view->list->list[i + view->top_line]);
        else
            text = do_strdup(empty);

        if (c_strlen(text) > view->n_col) {
	    buf = (char*)do_malloc(view->n_col*4 + 1);
            memset(buf, 0, view->n_col*4 + 1);
            c_strncpy(buf, text, view->n_col);
	    do_free(text);
            text = buf;
        }
        if (i == view->sel_line)
            wattron(view->win, COLOR_PAIR(C_COLOR_SELECT));

        sprintf(frm, "%%-%ds", (int)(view->n_col + (strlen(text) - c_strlen(text))));
        mvwprintw(view->win, i + top, 1, frm, text);
        if (i == view->sel_line)
            wattron(view->win, COLOR_PAIR(C_COLOR_DEFAULT));
        do_free(text);
    }
    wrefresh(view->win);
    c_text_view_refresh_footer(view);

}
static void c_text_view_get_continue(c_text_view_t *view)
{
    if ( view->list_ending_func ) {
        do_string_list_t *ret;
        ret = view->list_ending_func(view->list_ending_user_data);
        if ( ret ) {
            int i;
            for ( i = 0; i < ret->count; i++ )
                do_string_list_add_alloc(view->list, ret->list[i]);
            do_string_list_free(ret);
        }
    }
}
static void c_text_view_sel_move(c_text_view_t *view, int i)
{
    int n = i;

    if ( view->mode == 1 ) {
        if ( (view->sel_line + n)%2 ) {
            n += ( n < 0 ) ? -1 : 1;
        }
    }

    if ( view->sel_line + n > view->list->count - 1 - view->top_line )
        c_text_view_get_continue(view);


    if (view->sel_line + n > view->list->count - 1 - view->top_line)
        view->sel_line = view->list->count - 1 - view->top_line;
    else
        view->sel_line += n;

    if (view->sel_line < 0) {
        view->top_line += view->sel_line;
        view->sel_line = 0;
        if (view->top_line < 0)
            view->top_line = 0;
    }
    else
    if (view->sel_line > view->n_row - 1) {

        view->top_line += view->sel_line - view->n_row + 1;
        view->sel_line = view->n_row - 1;
        if (view->top_line >= view->list->count)
            view->top_line = view->list->count - 1;
    }
    c_text_view_refresh(view);
}
static void c_text_view_sel_page_move(c_text_view_t *view, int n)
{
    if (view->top_line + n*view->n_row*2 > view->list->count - 1)
        c_text_view_get_continue(view);

    if ( !view->list->count )
        return;

    if (view->top_line + n*view->n_row > view->list->count - 1) {
        if (view->top_line < view->list->count - view->n_row) {
            view->top_line = view->list->count - view->n_row;
        }
        if (view->top_line < 0)
            view->top_line = 0;
        view->sel_line = view->list->count - 1 - view->top_line;
    }
    else
        view->top_line += n*view->n_row;

    if (view->top_line < 0) {
        view->top_line = 0;
        view->sel_line = 0;
    }
    if (view->sel_line > view->list->count - 1 - view->top_line)
        view->sel_line = view->list->count - 1 - view->top_line;

    c_text_view_refresh(view);
}
static void c_text_view_sel_home_move(c_text_view_t *view, int up)
{
    if (up) {
        view->top_line = 0;
        view->sel_line = 0;
    }
    else {
        if (view->top_line < view->list->count - view->n_row) {
            view->top_line = view->list->count - view->n_row;
        }
        if (view->top_line < 0)
            view->top_line = 0;
        view->sel_line = view->list->count - 1 - view->top_line;
    }

    c_text_view_refresh(view);
}

#define KEY_M_SHIFT 0x1000
#define KEY_M_ALT   0x2000
#define KEY_M_CTRL  0x4000
#define KEY_M_MASK  0x7000

#define XCTRL(x) (KEY_M_CTRL | ((x) & 0x1F))
#define ALT_(x) (KEY_M_ALT | (unsigned int)(x))

static int c_text_view_check_movement_keys(int key, c_text_view_t *view)
{

    switch (key) {
    case KEY_UP:
    case XCTRL ('p'):
        c_text_view_sel_move(view, -1);
        break;

    case KEY_DOWN:
    case XCTRL ('n'):
        c_text_view_sel_move(view, 1);
        break;
    case KEY_PPAGE:
    case ALT_ ('v'):
        c_text_view_sel_page_move(view, -1);
        break;

    case KEY_NPAGE:
    case XCTRL ('v'):
        c_text_view_sel_page_move(view, 1);
        break;
    case KEY_HOME:
    case KEY_M_CTRL | KEY_HOME:
    case KEY_M_CTRL | KEY_PPAGE:
    case KEY_A1:
    case ALT_ ('<'):
        c_text_view_sel_home_move(view, 1);
        break;

    case KEY_END:
    case KEY_M_CTRL | KEY_END:
    case KEY_M_CTRL | KEY_NPAGE:
    case KEY_C1:
    case ALT_ ('>'):
        c_text_view_sel_home_move(view, 0);
        break;

    default:
        return 0;
    }
    return 1;
}
