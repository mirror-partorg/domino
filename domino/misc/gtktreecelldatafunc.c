#include "gtktreecelldatafunc.h"
#include <string.h>

//#include <gdk/gdkkeysyms.h>
#define RU_MONEY "р"
static gchar *do_money_format(double value, int abr)
{
    if ( value < 0.004 && value > -0.004 )
        return g_strdup("");

    char *buf, *ch, *retval, *ch1;
    if (abr)
        buf = g_strdup_printf("%.2f" RU_MONEY, value);
    else
        buf = g_strdup_printf("%.2f", value);
    gint len = strlen(buf);
    gint sign = (value > 0) ? 0 : 1;
    gint len_man = len - ((abr) ? strlen(RU_MONEY) : 0) - 3 - sign;
    retval = g_malloc0(len + 10 + (len_man % 3 + 1));
    ch = buf;
    ch1 = retval;
    while (*ch != '\0') {
        if (!((len_man - (ch - buf - sign)) % 3) &&
            (ch > buf + sign) &&
            (len_man - (ch - buf - sign) > 0))
            *ch1 = ' ',ch1++;
        *ch1 = *ch++, ch1++;
    }
    g_free(buf);
    return retval;
}
static gchar *do_big_int_format(int value)
{
    if ( !value )
        return strdup("");

    char *buf, *ch, *retval, *ch1;
    buf = g_strdup_printf("%d", value);
    gint len = strlen(buf);
    gint sign = (value > 0) ? 0 : 1;
    gint len_man = len - sign;
    retval = g_malloc0(len + 1 + (len_man % 3 + 1));
    ch = buf;
    ch1 = retval;
    while (*ch != '\0') {
        if (!((len_man - (ch - buf - sign)) % 3) &&
            (ch > buf + sign) &&
            (len_man - (ch - buf - sign) > 0))
            *ch1 = ' ',ch1++;
        *ch1 = *ch++, ch1++;
    }
    g_free(buf);
    return retval;
}
/*!!
static gchar *name_100[9] =
{"сто", "двести", "триста", "четыреста", "пятьсот",
"шестьсот", "семьсот", "восемьсот", "девятьсот"};
static gchar *name_11[10] =
{"десять", "одиннадцать", "двенадцать", "тринадцать", "четырнадцать",
"пятнадцать", "шестнадцать", "семнадцать", "восемнадцать", "девятнадцать"};
static gchar *name_10[8] =
{"двадцать", "тридцать", "сорок", "пятьдесят",
"шестьдесят", "семьдесят", "восемьдесят", "девяносто"};
static gchar *name_1[9] = {"один", "два", "три", "четыре", "пять", "шесть","семь", "восемь", "девять"};
static gchar *name_1j[9] = {"одна", "две", "три", "четыре", "пять", "шесть","семь", "восемь", "девять"};
static gchar *ind[5] = {"рубл", " тысяч", " миллион", " миллиард", " триллион"};
static gchar *suf[5][3] = {{"ь","я", "ей"},
                           {"а", "и", ""},
                           {"", "а", "ов"},
                           {"", "а", "ов"},
                           {"", "а", "ов"}};
static gchar *kopeck[] = {"копейка", "копейки", "копеек"};

static gchar *triada(const gchar *in, int tn, int abr)
{
    if (!strncmp(in, "   ", 3) ||
        !strncmp(in, "000", 3) ||
        !strncmp(in, "  0", 3) )
        return NULL;

    GString *string = g_string_new(NULL);

    if (in[0] != ' ' && in[0] != '0')
        g_string_append(string, name_100[in[0] - '1']);
    if (in[1] == '1')
        g_string_append_printf(string, " %s", name_11[in[2] - '0']);
    else {
        if (in[1] != ' ' && in[1] != '0')
            g_string_append_printf(string, " %s", name_10[in[1] - '2']);
        if (in[2] != '0') {
            if (tn == 1)
                g_string_append_printf(string, " %s", name_1j[in[2] - '1']);
            else
                g_string_append_printf(string, " %s", name_1[in[2] - '1']);
        }
    }
    if (tn || abr) {
        g_string_append_printf(string, " %s", ind[tn]);
        switch (in[2]) {
            case '1':
                g_string_append(string, suf[tn][0]);
                break;
            case '2':
            case '3':
            case '4':
                g_string_append(string, suf[tn][1]);
                break;
            default :
                g_string_append(string, suf[tn][2]);
        }
    }
    return g_string_free(string, FALSE);
}
static gchar *do_money_string(double value, int mantissa, int abr)
{
    gchar *buf, *ch, *val;
    gint i;
    GString *string = g_string_new(NULL);
    buf = g_strdup_printf("%18.2f", value);
    if (!mantissa)
        buf[strlen(buf)-2] = '\0';

    for (i = 4, ch = buf; i >= 0; i--, ch+=3) {
        val = triada(ch, i, abr);
        if (val) {
            if (string->len)
                g_string_append(string, " ");
            g_string_append(string, val);
        }
    }
    ch++;
    g_string_append_printf(string, " %s ", ch);
    if (mantissa) {
        switch (ch[1]) {
            case '1':
                g_string_append(string, kopeck[(ch[0] == '1') ? 2 : 0]);
                break;
            case '2':
            case '3':
            case '4':
                g_string_append(string, kopeck[(ch[0] == '1') ? 2 : 1]);
                break;
            default:
                g_string_append(string, kopeck[2]);
        }
    }
    g_free(buf);
    buf = g_string_free(string, FALSE);
    return buf;
}

static gchar *do_money_buh(double value)
{
    if (!value)
        return strdup("");
    char buf[500], *ch;
    sprintf(buf, "%.2f", value);
    ch = buf + strlen(buf) - 1;
    while (ch > buf ) {
        if (*ch == ',' || *ch == '.') {
            *ch = '-';
            break;
        }
        ch--;
    }
    return g_strdup(buf);
}*/
static gchar *do_rest_format(double value)
{
    if ( value > -0.0001 && value < 0.0001)
        return strdup("");
    char buf[500], *ch;
    sprintf(buf, "%.3f", value);
    ch = buf + strlen(buf) - 1;
    while (ch > buf && (*ch == '0' || *ch == ',' || *ch == '.')) {
        if (*ch == ',' || *ch == '.') {
            *ch = '\0';
            break;
        }
        *ch = '\0', ch--;
    }
    return g_strdup(buf);
}
static gchar *do_percent_format(double value)
{
    if (!value)
        return strdup("");
    char buf[500];//, *ch;
    sprintf(buf, "%.1f", value);
    /*ch = buf + strlen(buf) - 1;
    while (ch > buf && (*ch == '0' || *ch == ',' || *ch == '.')) {
        if (*ch == ',' || *ch == '.') {
            *ch = '\0';
            break;
        }
        *ch = '\0', ch--;
    }*/
    return g_strdup(buf);
}
/*!!
static gchar *do_first_alpha_upper(const gchar *name)
{
    gchar *buf = g_malloc(sizeof (gunichar) * strlen (name));
    gunichar  out;
    int nlen = 0, first_alpha = 1;
    gchar *p;
    for (p = (gchar*)name; p && *p !='\0'; p = (gchar*)g_utf8_next_char(p)) {
        out = g_utf8_get_char(p);
        if (first_alpha && g_unichar_isalpha(out)) {
            out = g_unichar_toupper (out);
            first_alpha = 0;
        }
        nlen += g_unichar_to_utf8(out, buf + nlen);
    }
    buf[nlen] = '\0';
    return buf;
}
*/
void gtk_tree_cell_data_func_rest(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, GPOINTER_TO_INT(data), &value);
    gchar *buf;
    buf = do_rest_format(g_value_get_double(&value));
    g_object_set(cell, "text", buf, NULL);
    g_free (buf);
}
void gtk_tree_cell_data_func_rest_int(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, GPOINTER_TO_INT(data), &value);
    gchar *buf;
    buf = do_rest_format(g_value_get_int(&value));
    g_object_set(cell, "text", buf, NULL);
    g_free (buf);
}
void gtk_tree_cell_data_func_big_int(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, GPOINTER_TO_INT(data), &value);
    gchar *buf;
    buf = do_big_int_format(g_value_get_int(&value));
    g_object_set(cell, "text", buf, NULL);
    g_free (buf);
}
void gtk_tree_cell_data_func_big_int_bold(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, GPOINTER_TO_INT(data), &value);
    gchar *buf, *v;
    buf = do_big_int_format(g_value_get_int(&value));
    v = g_strdup_printf("<b>%s</b>", buf);
    g_object_set(cell, "markup", v, NULL);
    g_free (buf);g_free(v);
}
void gtk_tree_cell_data_func_percent(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, GPOINTER_TO_INT(data), &value);
    gchar *buf, *text;
    buf = do_percent_format(g_value_get_double(&value)*100);
    if ( buf[0] != '\0') {
        text = g_strdup_printf("%s%%", buf);
        g_object_set(cell, "text", text, NULL);
        g_free (text);
    }
    else
        g_object_set(cell, "text", "", NULL);
    g_free (buf);
}
void gtk_tree_cell_data_func_percent_bold(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, GPOINTER_TO_INT(data), &value);
    gchar *buf, *text;
    buf = do_percent_format(g_value_get_double(&value)*100);
    if ( buf[0] != '\0') {
        text = g_strdup_printf("<b>%s%%</b>", buf);
        g_object_set(cell, "markup", text, NULL);
        g_free (text);
    }
    else
        g_object_set(cell, "text", "", NULL);
    g_free (buf);
}
void gtk_tree_cell_data_func_rest_bold(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, GPOINTER_TO_INT(data), &value);

    gchar *buf;
    gchar *markup;
    buf = do_rest_format(g_value_get_double(&value));
    markup = g_markup_printf_escaped("<b>%s</b>", buf);
    g_object_set(cell, "markup", markup, NULL);
    g_free (markup);
    g_free (buf);
}
void gtk_tree_cell_data_func_money(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, GPOINTER_TO_INT(data), &value);
    gchar *buf;
    buf = do_money_format(g_value_get_double(&value), TRUE);
    g_object_set(cell, "text", buf, NULL);
    g_free (buf);
}
void gtk_tree_cell_data_func_money_no_negavite(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, GPOINTER_TO_INT(data), &value);
    gchar *buf;
    if ( g_value_get_double(&value) > 0 ) {
        buf = do_money_format(g_value_get_double(&value), TRUE);
        g_object_set(cell, "text", buf, NULL);
        g_free (buf);
    }
    else
        g_object_set(cell, "text", "", NULL);
}
void gtk_tree_cell_data_func_money_bold(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, GPOINTER_TO_INT(data), &value);
    gchar *buf;
    gchar *markup;
    buf = do_money_format(g_value_get_double(&value), TRUE);
    markup = g_markup_printf_escaped("<b>%s</b>", buf);
    g_object_set(cell, "markup", markup, NULL);
    g_free (markup);
    g_free (buf);
}
void gtk_tree_cell_data_func_big_int_bold_depth1(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GtkTreePath *path;

    path = gtk_tree_model_get_path(tree_model, iter);
    if ( gtk_tree_path_get_depth(path) == 1 )
         gtk_tree_cell_data_func_big_int_bold(tree_column, cell,tree_model,iter,data);
    else
         gtk_tree_cell_data_func_big_int(tree_column, cell,tree_model,iter,data);

    gtk_tree_path_free(path);
}
void gtk_tree_cell_data_func_big_int_bold_total(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GtkTreePath *path;
    GtkTreeIter new_iter;

    path = gtk_tree_model_get_path(tree_model, iter);
    gtk_tree_path_next(path);
    if ( gtk_tree_path_get_depth(path) == 1 &&
         !gtk_tree_model_get_iter(tree_model, &new_iter, path) )
         gtk_tree_cell_data_func_big_int_bold(tree_column, cell,tree_model,iter,data);
    else
         gtk_tree_cell_data_func_big_int(tree_column, cell,tree_model,iter,data);

    gtk_tree_path_free(path);
}
void gtk_tree_cell_data_func_money_bold_total(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GtkTreePath *path;
    GtkTreeIter new_iter;

    path = gtk_tree_model_get_path(tree_model, iter);
    gtk_tree_path_next(path);
    if ( gtk_tree_path_get_depth(path) == 1 &&
         !gtk_tree_model_get_iter(tree_model, &new_iter, path) )
         gtk_tree_cell_data_func_money_bold(tree_column, cell,tree_model,iter,data);
    else
         gtk_tree_cell_data_func_money(tree_column, cell,tree_model,iter,data);

    gtk_tree_path_free(path);
}
void gtk_tree_cell_data_func_money_bold_depth1(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GtkTreePath *path;

    path = gtk_tree_model_get_path(tree_model, iter);
    if ( gtk_tree_path_get_depth(path) == 1 )
         gtk_tree_cell_data_func_money_bold(tree_column, cell,tree_model,iter,data);
    else
         gtk_tree_cell_data_func_money(tree_column, cell,tree_model,iter,data);

    gtk_tree_path_free(path);
}
void gtk_tree_cell_data_func_percent_bold_total(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GtkTreePath *path;
    GtkTreeIter new_iter;

    path = gtk_tree_model_get_path(tree_model, iter);
    gtk_tree_path_next(path);
    if ( gtk_tree_path_get_depth(path) == 1 &&
         !gtk_tree_model_get_iter(tree_model, &new_iter, path) )
         gtk_tree_cell_data_func_percent_bold(tree_column, cell,tree_model,iter,data);
    else
         gtk_tree_cell_data_func_percent(tree_column, cell,tree_model,iter,data);

    gtk_tree_path_free(path);
}
void gtk_tree_cell_data_func_percent_bold_depth1(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GtkTreePath *path;

    path = gtk_tree_model_get_path(tree_model, iter);
    if ( gtk_tree_path_get_depth(path) == 1 )
         gtk_tree_cell_data_func_percent_bold(tree_column, cell,tree_model,iter,data);
    else
         gtk_tree_cell_data_func_percent(tree_column, cell,tree_model,iter,data);

    gtk_tree_path_free(path);
}
void gtk_tree_cell_data_func_money_bold_no_negative(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, GPOINTER_TO_INT(data), &value);
    gchar *buf;
    gchar *markup;
    if ( g_value_get_double(&value) > 0 ) {
        buf = do_money_format(g_value_get_double(&value), TRUE);
        markup = g_markup_printf_escaped("<b>%s</b>", buf);
        g_object_set(cell, "markup", markup, NULL);
        g_free (markup);
        g_free (buf);
    }
    else
        g_object_set(cell, "markup", "", NULL);
}

//!!static GList* valuta_list = NULL;
//!!static const gchar *empty_str = "";

void gtk_tree_cell_data_func_money_with_sign(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GValue value = { 0, };
    GValue code = { 0, };
    gchar *buf;

    gtk_tree_model_get_value(tree_model, iter, GPOINTER_TO_INT(data), &value);
    gtk_tree_model_get_value(tree_model, iter, GPOINTER_TO_INT(data) + 1, &code);

    buf = do_money_format(g_value_get_double(&value), FALSE);
    gchar *buf1 = g_strdup_printf("%s%s", buf, RU_MONEY);
    g_object_set(cell, "text", buf1, NULL);
    g_free (buf1);
    g_free (buf);
}

void gtk_tree_cell_data_func_money_no_abr(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, GPOINTER_TO_INT(data), &value);
    gchar *buf;
    buf = do_money_format(g_value_get_double(&value), FALSE);
    g_object_set(cell, "text", buf, NULL);
    g_free (buf);
}

void gtk_tree_cell_data_func_date_time(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    struct tm tm;
    time_t now;
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, GPOINTER_TO_INT(data), &value);
    gchar *buf;
    now = g_value_get_long(&value);
    tm = *localtime(&now);
    buf = g_strdup_printf("%2.2d/%2.2d/%2.2d %2.2d:%2.2d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year - 100, tm.tm_hour, tm.tm_min);
    g_object_set(cell, "text", buf, NULL);
    g_free (buf);
}
void gtk_tree_cell_data_func_date_time_date_only(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    struct tm tm;
    time_t now;
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, GPOINTER_TO_INT(data), &value);
    gchar *buf;
    now = g_value_get_long(&value);
    if (now != -1) {
        tm = *localtime(&now);
        buf = g_strdup_printf("%2.2d/%2.2d/%2.2d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year - 100);
        g_object_set(cell, "text", buf, NULL);
        g_free (buf);
    }
    else
        g_object_set(cell, "text", "", NULL);
}
void gtk_tree_cell_data_func_julian_lifetime(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    guint julian_day = 0;
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, GPOINTER_TO_INT(data), &value);
    julian_day = g_value_get_uint(&value);
    if ( julian_day ) {
        GDate *date;
        gchar buf[20];
        date = g_date_new_julian(julian_day);
        g_date_strftime(buf, sizeof(buf)-1, "%b %y", date);
        g_object_set(cell, "text", buf, NULL);
        g_date_free(date);
    }
    else
        g_object_set(cell, "text", "", NULL);
}
gchar *do_date_time_format(time_t t)
{
    struct tm tm;
    tm = *localtime(&t);
    return g_strdup_printf("%2.2d/%2.2d/%2.2d %2.2d:%2.2d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year - 100, tm.tm_hour, tm.tm_min);
}
void gtk_tree_cell_data_func_pixbuf_bool(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, GPOINTER_TO_INT(data), &value);
    if (g_value_get_boolean(&value))
#if GTK_MAJOR_VERSION > 2
        g_object_set(cell, "icon-name", "object-select-symbolic", NULL);
#else
        g_object_set(cell, "icon-name", GTK_STOCK_YES, NULL);
#endif
    else
        g_object_set(cell, "icon-name", NULL, NULL);
}
void gtk_tree_cell_data_func_pixbuf_warning(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, GPOINTER_TO_INT(data), &value);
    if (g_value_get_boolean(&value))
#if GTK_MAJOR_VERSION > 2
        g_object_set(cell, "icon-name", "dialog-warning-symbolic", NULL);
#else
        g_object_set(cell, "icon-name", GTK_STOCK_DIALOG_WARNING, NULL);
#endif
    else
        g_object_set(cell, "icon-name", NULL, NULL);
}
void gtk_tree_cell_data_func_pixbuf_error (GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, GPOINTER_TO_INT(data), &value);
    if (g_value_get_boolean(&value))
#if GTK_MAJOR_VERSION > 2
        g_object_set(cell, "icon-name", "software-update-urgent-symbolic", NULL);
#else
        g_object_set(cell, "icon-name", GTK_STOCK_DIALOG_WARNING, NULL);
#endif
    else
        g_object_set(cell, "icon-name", NULL, NULL);
}

void gtk_tree_cell_data_func_pixbuf_connect(GtkTreeViewColumn *tree_column,
                                                         GtkCellRenderer *cell,
                                                         GtkTreeModel *tree_model,
                                                         GtkTreeIter *iter,
                                                         gpointer data)
{
    GValue value = { 0, };
    gtk_tree_model_get_value(tree_model, iter, GPOINTER_TO_INT(data), &value);
#if GTK_MAJOR_VERSION > 2
    switch (g_value_get_int(&value)) {
        case 0 :
            g_object_set(cell, "icon-name", "network-wired-acquiring-symbolic", NULL);
            break;
        case 1:
            g_object_set(cell, "icon-name", "network-wired-symbolic", NULL);
            break;
        case -1:
            g_object_set(cell, "icon-name", "network-wired-disconnected-symbolic", NULL);
            break;
        default:
            g_object_set(cell, "icon-name", NULL, NULL);
            break;
    };
#else
    switch (g_value_get_int(&value)) {
        case 0 :
            g_object_set(cell, "icon-name", GTK_STOCK_REFRESH, NULL);
            break;
        case 1:
            g_object_set(cell, "icon-name", GTK_STOCK_YES, NULL);
            break;
        case -1:
            g_object_set(cell, "icon-name", GTK_STOCK_NO, NULL);
            break;
        default:
            g_object_set(cell, "icon-name", NULL, NULL);
            break;
    };
#endif
}
