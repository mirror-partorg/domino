#include "c_curses.h"
#include <signal.h>
#include <sys/ioctl.h>
#include <domino.h>
#include <slang.h>
#include <termios.h>

static int curses_init_ = 0;
static do_list_t *widgets = NULL;

static void init_my_color()
{
    start_color();
    SLtt_set_color (1, (char *) "", "white", "black");
    SLtt_set_color (2, (char *) "", "black", "color6");
    //SLtt_set_color (2, (char *) "", "white", "green");
    //init_pair(1, COLOR_WHITE, COLOR_BLACK);
    //init_pair(2, COLOR_BLACK, COLOR_GREEN);
    //init_color(COLOR_BLUE, 0, 0, 735);
    //init_color(COLOR_GREEN, 0, 735, 735);

}
inline int  c_curses_get_col_n()
{
    return COLS;
}

static void sigwinch_handler(int k)
{
    int i;

/*    struct winsize winsz;

    winsz.ws_col = winsz.ws_row = 0;
    // Ioctl on the STDIN_FILENO
    ioctl (0, TIOCGWINSZ, &winsz);
    if (winsz.ws_col && winsz.ws_row){
        COLS = winsz.ws_col;
        LINES = winsz.ws_row;
    }*/
    /*struct winsize size;
    ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size);
    resizeterm(size.ws_row, size.ws_col);
    */
    endwin();
    initscr();
    noecho();
    cbreak();
    curs_set(0);
    init_my_color();
    for (i = 0; i < widgets->count; i++)
        c_text_view_resize(widgets->list[i]);
}

static void setup_sigwinch ()
{
    struct sigaction act, oact;
    act.sa_handler = sigwinch_handler;
    sigemptyset (&act.sa_mask);
    act.sa_flags = 0;
    sigaction (SIGWINCH, &act, &oact);
}
void disable_extended_io(void)
{
    struct termios term;

    tcgetattr(0, &term);
    term.c_lflag &= ~IEXTEN;
    term.c_oflag &= ~OPOST;
    tcsetattr(0, TCSANOW, &term);
}

void terminal_init(void)
{
    static struct termios newterm;
    static bool newterm_set = FALSE;

    if (!newterm_set) {

    raw();
    nonl();
    noecho();
    disable_extended_io();
    //if (ISSET(PRESERVE))
      //  enable_flow_control();

    //disable_signals();
    //if (!ISSET(PRESERVE))
/*        disable_flow_control();

        tcgetattr(0, &newterm);
        newterm_set = TRUE;*/
    } else
    tcsetattr(0, TCSANOW, &newterm);
}

void c_curses_init()
{
    if (curses_init_)
        return;
    widgets = do_list_new(0);
    initscr();
    terminal_init();
    noecho();
    cbreak();
    curs_set(0);
    init_my_color();
    setup_sigwinch ();
    curses_init_ = 1;
}
void c_curses_stop()
{
    if (!curses_init_)
        return;
    curs_set(1);
    endwin();
    do_list_free(widgets);
    curses_init_ = 0;
}

int c_curses_wgetch(curses_window_t *win)
{
    int ch, i, len;
    int32_t retval;

    ch = wgetch(win);
    len = c_chlen(ch);
    retval = ch;
    for (i = 1; i < len; i++) {
        ch = wgetch(win);
        if (i < 4) {
            //retval <<= 8;
            retval |= ch << 8*i;
        }
    }
    return retval;
}
void c_curses_insert_widget(void *widget)
{
    do_list_add(widgets, widget);
}
void c_curses_delete_widget(void *widget)
{
    int i;
    for (i = 0; i < widgets->count; i++)
        if (widgets->list[i] == widget) {
            do_list_delete(widgets, i);
            break;
        }
}
