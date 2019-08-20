#include "misc.h"
#include "do_scan.h"
#include "do_message.h"

gboolean str_iqual(const gchar *s1, const gchar *s2)
{
    gchar *s1_u = NULL,*s2_u = NULL;
    gboolean res;
    if ( s1 )
        s1_u = g_utf8_strup(s1, -1);
    if ( s2)
        s2_u = g_utf8_strup(s2, -1);
    res = !g_strcmp0(s1_u, s2_u);
    g_free(s1_u);g_free(s2_u);
    return res;

}
static DoProgressInfo *progress = NULL;
DoProgressInfo *get_progress()
{
    return progress;
}
# define ISLEAP(year)	((year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0))
#define	SECS_PER_HOUR (60 * 60)
static const unsigned short int __mon_yday[2][13] = {
    { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
    { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
};
static void day_of_the_week (struct tm *tm)
{
  int corr_year = 1900 + tm->tm_year - (tm->tm_mon < 2);
  int wday = (-473
	      + (365 * (tm->tm_year - 70))
	      + (corr_year / 4)
	      - ((corr_year / 4) / 25) + ((corr_year / 4) % 25 < 0)
	      + (((corr_year / 4) / 25) / 4)
	      + __mon_yday[0][tm->tm_mon]
	      + tm->tm_mday - 1);
  tm->tm_wday = ((wday % 7) + 7) % 7;
}

static void day_of_the_year (struct tm *tm)
{
  tm->tm_yday = (__mon_yday[ISLEAP(1900 + tm->tm_year)][tm->tm_mon]
		 + (tm->tm_mday - 1));
}
gboolean strpdatetime(const gchar *str, struct tm *tm)
{
    tm->tm_isdst = 0; // no daylight
    memset(tm, 0, sizeof(*tm));

    char *cp = (char*)str;
    tm->tm_year = atoi(cp);
    if (tm->tm_year > 1000)
        tm->tm_year -= 1900;
    else
        tm->tm_year += 100;
    while (*cp && *cp != '/' && *cp != '-')
      ++cp;
    if (*cp) ++cp;
    tm->tm_mon = atoi(cp) - 1;
    while (*cp && *cp != '/' && *cp != '-')
      ++cp;
    if (*cp) ++cp;
    tm->tm_mday = atoi(cp) ;

    day_of_the_week(tm);
    day_of_the_year(tm);
    return (tm->tm_year > -70 && tm->tm_year < 10000) && (tm->tm_mon >=0 && tm->tm_mon <12) && (tm->tm_mday > 0 && tm->tm_mday < 32);

}
/*
gchar *strfdatetime(struct tm *tm)
{
    gchar *res;
    res = g_malloc0(100);
    return res;
};
*/

void set_progress(DoProgressInfo *prog)
{
    progress = prog;
}
GCancellable  *cancellable=NULL;
GCancellable  *get_cancellable()
{
    return cancellable;
}
void set_cancellable(GCancellable *c)
{
    cancellable = c;
}

static GKeyFile *key_file = NULL;
static gchar *key_file_path;
gboolean config_file_init()
{
    gboolean res;

    key_file_path = g_build_path (G_DIR_SEPARATOR_S,
                     g_get_user_config_dir (),
                     "domino",
                     "exchange-1c-settings",
                     NULL);

    key_file = g_key_file_new ();
    res = g_key_file_load_from_file (key_file,
                                    key_file_path,
                                    G_KEY_FILE_KEEP_COMMENTS|G_KEY_FILE_KEEP_TRANSLATIONS,
                                 NULL);
    return res;

}
gboolean config_file_save()
{
    if ( !key_file )
        return FALSE;
    gsize content_size = 0;
    gchar *contents = g_key_file_to_data (key_file, &content_size, NULL);
    g_file_set_contents (key_file_path, contents, content_size, NULL);
    g_free (contents);
    return TRUE;
}
void window_configure_save(GtkWidget *widget, gchar *path)
{

    if ( !key_file )
        return;
    GdkWindowState state = gdk_window_get_state (gtk_widget_get_window(widget));

    if (!(state &
        (GDK_WINDOW_STATE_MAXIMIZED | GDK_WINDOW_STATE_FULLSCREEN))) {
        GtkAllocation allocation;
        gtk_widget_get_allocation(GTK_WIDGET(widget), &allocation);
        g_key_file_set_integer(key_file, path, "width", allocation.width);
        g_key_file_set_integer(key_file, path, "height", allocation.height);
        config_file_save();
    }
}
void window_configure_init(GtkWidget *widget, gchar *path, gint width, gint height)
{
    if ( !key_file )
        return;
    gint w,h;
    w = g_key_file_get_integer(key_file, path, "width", NULL);
    h = g_key_file_get_integer(key_file, path, "height", NULL);
    if ( !w )
        w=width,h=height;
    gtk_window_resize(GTK_WINDOW(widget), w, h);
}
GSList *text_to_lexs(const gchar *text)
{
    if ( !text )
        return NULL;

    DoScan *scan;
    DoScanLexemType type;
    GSList *res = NULL;
    gchar *value;
    gchar *text_up;
    text_up = g_utf8_strup(text, -1);

    scan = do_scan_new(text_up);

    while ( (type = do_scan_step(scan)) != DO_SCAN_LEXEM_END ) {
        switch ( type ) {
            case DO_SCAN_LEXEM_IDENT:
            case DO_SCAN_LEXEM_CINTEGER:
                g_object_get(scan, "last-lexem", &value, NULL);
                if ( text_length(value) > 1 )
                    res = g_slist_append(res, g_strdup(value));
                break;
            case DO_SCAN_LEXEM_CSTRING: {
                GSList *tmp;
                g_object_get(scan, "last-lexem", &value, NULL);
                tmp = text_to_lexs(value);
                res = g_slist_concat(res, tmp);

            }
            default:
                break;

        }

    }
    g_free(text_up);
    return res;
}
gboolean lexs_compare(GSList *l1, GSList *l2, gint min_match)
{
    gint match = 0;
    GSList *l, *k;
    for ( l=l1; l; l=l->next ) {
        for ( k=l2; k; k=k->next ) {
            if ( !g_strcmp0(k->data, l->data) ) {
                match++;
                break;
            }
        }
        if ( match >= min_match )
            break;
    }
    return match >= min_match;
}
typedef struct {
    GSList *name;
    gint col;
    gint col_cache;
    GSList *res;
    gint min_match;
} FindSt;

static gboolean find_func(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, FindSt *find_st);
GtkTreePath *misc_find_by_name_from_model(GtkTreeModel *model, GSList *MiscGtkTreeModelFindColumns)
{
    GSList *l;
    FindSt find_st;
    for ( l = MiscGtkTreeModelFindColumns; l; l=l->next ) {
        MiscGtkTreeModelFindColumn *col;
        col = l->data;
        find_st.name = text_to_lexs(col->text);
        find_st.col = col->column;
        find_st.col_cache = col->column_cache;
        find_st.min_match = 1;
        while ( TRUE ) {
            find_st.res = NULL;
            gtk_tree_model_foreach(model, (GtkTreeModelForeachFunc)find_func, &find_st);
            if ( g_slist_length(find_st.res) < 2 )
                break;
            find_st.min_match++;
        }
        g_slist_foreach(find_st.name, (GFunc)g_free, NULL);
        g_slist_free(find_st.name);
        if ( g_slist_length(find_st.res) == 1 )
            break;
    }

    if ( find_st.res )
        return find_st.res->data;
    return NULL;
}
static gboolean find_func(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, FindSt *find_st)
{
    GSList *text;
    if ( find_st->col_cache != -1 ) {
        GValue value = {0,};
        gtk_tree_model_get_value(model, iter, find_st->col_cache, &value);
        text = g_value_get_pointer(&value);
        if ( !text ) {
            GValue value = {0,};
            gtk_tree_model_get_value(model, iter, find_st->col, &value);
            text = text_to_lexs(g_value_get_string(&value));
            gtk_list_store_set(GTK_LIST_STORE(model), iter,
                               find_st->col_cache, text,
                               -1);
        }
    }
    else {
        GValue value = {0,};
        gtk_tree_model_get_value(model, iter, find_st->col, &value);
        text = text_to_lexs(g_value_get_string(&value));
    }

    if ( lexs_compare(text, find_st->name, find_st->min_match) )
        find_st->res = g_slist_append(find_st->res, gtk_tree_path_copy(path));

    if ( find_st->col_cache == -1 ) {
        g_slist_foreach(text, (GFunc)g_free, NULL);
        g_slist_free(text);
    }
    return FALSE;
}
gint text_length(const gchar *text)
{
    gchar *p;
    gint res = 0;
    p = (gchar*)text;

    for ( ;*p != '\0'; p = g_utf8_next_char (p), res++ );

    return res;
}
void wait_for(gboolean *event)
{
    while ( *event ) g_usleep(5000);
}
