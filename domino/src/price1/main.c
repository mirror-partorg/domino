#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <libpq-fe.h>
#include "../misc/gtk_tree_cell_data_func.h"

static GtkWidget *price_widget_new(const gchar *conn_str, const gchar *firm_nick);
static GtkWidget *price_view_new(const gchar *conn_str, const gchar *firm_nick);
static GtkTreeModel *price_model_new(const gchar *conn_str, const gchar *firm_nick, gchar ***values);
static void do_log(const char *msgfmt, ...);
static gchar *settings_filename;
static GKeyFile *settings;
//static gboolean win_delete_cb(GtkWindow *win);
static void win_set_geometry(GtkWindow *win);
static void settings_init();

#define DEFAULT_COLUMNS "order,product_name,price,stock,lifetime,firm_name,manufactor_name"
#define GEOMETRY_REGEX "(?P<width>[0-9]+)x(?P<height>[0-9]+)\\+(?P<left>[0-9]+)\\+(?P<top>[0-9]+)"


int main (int argc, char *argv[])
{
  GtkWidget *win = NULL;
  GtkWidget *price = NULL;

  /* Initialize GTK+ */
  g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, (GLogFunc) gtk_false, NULL);
  gtk_init (&argc, &argv);
  settings_init();
  g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, g_log_default_handler, NULL);



  /* Create the main window */
  win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_container_set_border_width (GTK_CONTAINER (win), 8);
  gtk_window_set_title (GTK_WINDOW (win), "Hello World");
  gtk_window_set_position (GTK_WINDOW (win), GTK_WIN_POS_CENTER);
  gtk_widget_realize (win);
//  g_signal_connect (win, "delete-event", G_CALLBACK(win_delete_cb), NULL);
  g_signal_connect (win, "destroy", G_CALLBACK(gtk_main_quit), NULL);

  price = price_widget_new("host=database dbname=domino user=domino password=trust","tta");
  /* Create a vertical box with buttons */
  gtk_container_add (GTK_CONTAINER (win), price);

  /* Enter the main loop */
  win_set_geometry(GTK_WINDOW(win));

  gtk_widget_show_all (win);
  gtk_main ();
  return 0;
}
#define MAX_LOG_LINE 1024
static void do_log(const char *msgfmt, ...)
{
    va_list ap;
    char *pos, message[MAX_LOG_LINE+1];
    int sz, len = 0;
    time_t t;

    /* timestamp */
    t = time(NULL);
    pos = ctime(&t);
    sz = strlen(pos);
    /* chop off the \n */
    pos[sz-1]=' ';

    message[len] = '\0';

    /* find the end and attach the rest of the msg */
    for (pos = message; *pos != '\0'; pos++); /*empty statement */
    sz = pos - message;
    va_start(ap, msgfmt);
    vsnprintf(pos, MAX_LOG_LINE - sz, msgfmt, ap);
    va_end(ap);
    fprintf(stderr, "%s", message);
    fprintf(stderr, "\n");
    fflush(stderr);
}


typedef struct _HandbookRecord HandbookRecord;
struct _HandbookRecord {
    gint code;
    gchar *name;
};
static void  handbook_record_clear_func(gpointer data)
{
    do_log("clear");
    g_free(((HandbookRecord*)data)->name);
}
typedef struct {
    gchar *name;
    GType type;
    gboolean visible;
    gchar *title;
    GtkTreeCellDataFunc func;
    gfloat xalign;
} field_t;

static const field_t fields[] =
{
  {"product_code",G_TYPE_STRING,FALSE,"",NULL,0},
  {"manufactor_code",G_TYPE_INT,FALSE,"",NULL,0},
  {"lifetime",G_TYPE_UINT, TRUE,"Срок",gtk_tree_cell_data_func_julian_lifetime,0.5},
  {"product_name",G_TYPE_STRING,TRUE,"Наименование",NULL,0},
  {"manufactor_name",G_TYPE_STRING,TRUE,"Производитель",NULL,0},
  {"price",G_TYPE_DOUBLE,TRUE,"Цена",gtk_tree_cell_data_func_money,1},
  {"stock",G_TYPE_INT,TRUE,"Остаток",gtk_tree_cell_data_func_rest_int,0.5},
  {"min",G_TYPE_INT,FALSE,"",NULL,0},
  {"parcel",G_TYPE_INT,FALSE,"",NULL,0},
  {"firm_name",G_TYPE_STRING,TRUE,"Поставщик",NULL,0},
  {"firm_code",G_TYPE_INT,FALSE,"",NULL,0},
  {"order",G_TYPE_INT,TRUE,"Заказ",gtk_tree_cell_data_func_rest_int,0.5},
};

static GtkTreeModel *price_model_new(const gchar *conn_str, const gchar *firm_nick, gchar ***values)
{
    GtkListStore *model;
    int i,firm,j;

    GType types[G_N_ELEMENTS(fields)];
    for ( i = 0; i < G_N_ELEMENTS(fields); i++)
        types[i] = fields[i].type;

    model = gtk_list_store_newv(G_N_ELEMENTS(fields), (GType*)types);

    do_log("start read db");
    PGconn *conn;
    PGresult *res;
    int nsupplier, nrow;
    GArray *suppliers = NULL;
    GArray *indexes = NULL;
    GHashTable *suppliers_;
    gchar *sql;
    gchar *str;
    gchar **names;

    conn = PQconnectdb(conn_str);

    res = PQexec(conn, "SELECT code,nick FROM get_firms() WHERE nick <> ''");
    nrow = PQntuples(res);
    for ( i = 0; i < nrow; i++ ) {
        gchar *value;
        value = PQgetvalue(res, i, 1);
        if ( !g_strcmp0(value, firm_nick) ) {
            firm = atoi(PQgetvalue(res, i, 0));
            break;
        }
    }
    PQclear(res);
    res = PQexec(conn, "SELECT code,name FROM get_suppliers() WHERE nick <> ''");
    nsupplier = PQntuples(res);
    suppliers = g_array_sized_new (FALSE,FALSE,sizeof(HandbookRecord),nsupplier);
    g_array_set_clear_func(suppliers, handbook_record_clear_func);
    indexes = g_array_sized_new (FALSE,FALSE,sizeof(gint),nsupplier);
    for ( i = 0; i < nsupplier; i++ ) {
        g_array_index (suppliers, HandbookRecord, i).code = atoi(PQgetvalue(res, i, 0));
        g_array_index (suppliers, HandbookRecord, i).name = g_strdup(PQgetvalue(res, i, 1));
    }
    PQclear(res);
    suppliers_ = g_hash_table_new(g_int_hash,g_int_equal);
	for ( i =0; i < nsupplier; i++ ) {
        sql = g_strdup_printf("SELECT price_index,price_time FROM price_price WHERE supplier_code=%d AND firm_code=%d ORDER BY price_time DESC, price_index DESC LIMIT 1" , g_array_index (suppliers, HandbookRecord, i).code,firm);
        res = PQexec(conn, sql);
        if ( PQntuples(res) ) {
            g_array_index (indexes, gint, i) = atoi(PQgetvalue(res, 0, 0));
            g_hash_table_insert(suppliers_, &g_array_index (indexes, gint, i), &g_array_index (suppliers, HandbookRecord, i));
        }
        else
           g_array_index (indexes, gint, i) = 0;
        PQclear(res);
        g_free(sql);
    }
    time_t t1 = time(NULL);

    do_log("read");
    str=NULL;
	for ( i =0; i < nsupplier; i++ )
        if ( g_array_index (indexes, gint, i) ) 	{
            gchar *buf;
            if ( str ) {
                buf = g_strdup_printf("%s,%d",str,g_array_index (indexes, gint, i));
                g_free(str);
                str=buf;
            }
            else
                str=g_strdup_printf("%d", g_array_index (indexes, gint, i));
        }

    sql = g_strdup_printf(
                "SELECT"
				"  a.product_code,a.manufactor_code,a.lifetime,a.product_name,"
				"  CASE WHEN a.manufactor_code <> 0 THEN c.manufactor_name ELSE a.manufactor_name END AS manufactor_name,"
				"  a.price,a.stock,a.quant_min,a.quant_parcel,a.price_index"
				" FROM"
				" (SELECT b.product_name,b.manufactor_name,a.* FROM price_position as a, price_goods as b"
				"   WHERE a.price_index in (%s) AND a.product_code=b.product_code AND b.price_index=a.price_index)"
				"   AS a"
				" LEFT JOIN price_manufactor as c ON (a.price_index=c.price_index AND a.manufactor_code=c.manufactor_code)"
				" ORDER BY UPPER(product_name),manufactor_name,lifetime DESC",str);
    res = PQexec(conn, sql);
    nrow = PQntuples(res);
    names = g_new(gchar*, nrow+1);
    names[nrow] = NULL;
    *values = names;
    for ( i = 0; i < nrow; i++ ) {
        HandbookRecord *supplier;
        GtkTreeIter iter;
        gint index;
        gtk_list_store_append(model, &iter);
        names[i] = g_utf8_strup(PQgetvalue(res, i, 3), -1);
        //*list = g_list_append(*list, g_strdup(PQgetvalue(res, i, 3)));
        for ( j = 0; j < G_N_ELEMENTS(fields)-2; j++ ) {
            if ( fields[j].type == G_TYPE_STRING )
                gtk_list_store_set(model, &iter, j, PQgetvalue(res, i, j),-1);
            else
            if ( fields[j].type == G_TYPE_INT )
                gtk_list_store_set(model, &iter, j, atoi(PQgetvalue(res, i, j)),-1);
            else
            if ( fields[j].type == G_TYPE_UINT ) {
                gchar **val;
                guint julian_day=0;
                GDate *date;
                val = g_strsplit(PQgetvalue(res, i, j),"-",-1);
                if ( g_strv_length(val) == 3) {
                    date = g_date_new_dmy(atoi(val[2]),atoi(val[1]),atoi(val[0]));
                    if ( date )  {
                        julian_day = g_date_get_julian(date);
                        g_date_free(date);
                    }
                }

                gtk_list_store_set(model, &iter, j, julian_day, -1);
                g_strfreev(val) ;
            }
            else
                gtk_list_store_set(model, &iter, j, atof(PQgetvalue(res, i, j)),-1);
        }
        index = atoi(PQgetvalue(res, i, 9));
        supplier = g_hash_table_lookup(suppliers_, &index);
        if ( supplier )
            gtk_list_store_set(model, &iter, 10, supplier->code,9, supplier->name,-1);
    }
    PQclear(res);
    g_free(sql);
    time_t t2 = time(NULL);
    do_log("sec %d",t2-t1);
    g_array_free(suppliers, FALSE);
    g_array_unref(indexes);

    return GTK_TREE_MODEL(model);

}
static void settings_init()
{
    settings_filename = g_build_filename(g_get_user_config_dir(),"domino","price.cfg",NULL);
    settings = g_key_file_new();
    g_key_file_load_from_file(settings, settings_filename, 0, NULL);

}
/*
static void price_view_destroy_cb(GtkTreeView *view, GHashTable *columns)
{
    GList *cols, *col;
    cols = gtk_tree_view_get_columns(view);
    gchar *names = NULL, *name, *sname;

    for ( col = cols; col; col=col->next ) {
        gchar *buf;
        name = (char*)g_object_get_data(G_OBJECT(col->data), "name");
        if ( names ) {
            buf = g_strdup_printf("%s,%s", names, name);
            g_free(names);
            names = buf;
        }
        else
            names = g_strdup(name);
        sname = g_strdup_printf("column-%s-width", name);
        g_key_file_set_integer(settings, "PriceView", sname, gtk_tree_view_column_get_width(col->data));
        g_free(sname);
    }
    g_key_file_set_string(settings, "PriceView", "columns", names);
    g_key_file_save_to_file(settings, settings_filename, NULL);

}
static gboolean win_delete_cb(GtkWindow *win)
{
    gchar *geometry;
    gint  width, height, x ,y;
    gboolean is_maximized;

    if ( gtk_widget_get_window(GTK_WIDGET(win)) ) {

        gtk_window_get_size(win, &width, &height);
        gtk_window_get_position(win, &x, &y);
        is_maximized = gdk_window_get_state(gtk_widget_get_window(GTK_WIDGET(win))) & GDK_WINDOW_STATE_MAXIMIZED;

        geometry = g_strdup_printf("%dx%d+%d+%d",width,height,x,y);

		if ( !is_maximized )
            g_key_file_set_string(settings, "test-price-win","geometry",geometry);
        g_key_file_set_boolean(settings, "test-price-win","maximized",is_maximized);
    }
    return FALSE;
}*/
static void win_set_geometry(GtkWindow *win)
{
    //!!gchar *geometry;
    //!!gint  width, height, x ,y;
    gboolean is_maximized;
    //!!GRegex *re = NULL;
    //!!GMatchInfo *match_info = NULL;

    is_maximized = g_key_file_get_boolean(settings,"test-price-win","maximized",NULL);
	if ( is_maximized )
        gtk_window_maximize(win);
    else
        gtk_window_unmaximize(win);

	/*!!geometry = g_key_file_get_string(settings, "test-price-win","geometry", NULL);
	if ( geometry ) {
        re = g_regex_new(GEOMETRY_REGEX,
                         G_REGEX_MULTILINE | G_REGEX_NEWLINE_CR, 0, NULL);
        g_regex_match (re, geometry, 0, &match_info);
        if ( g_match_info_matches (match_info) ) {
            char *value;
            value = g_match_info_fetch_named(match_info, "width");
            if ( value )
                width = atoi(value);
            value = g_match_info_fetch_named(match_info, "height");
            if ( value )
                height = atoi(value);
            value = g_match_info_fetch_named(match_info, "left");
            if ( value )
                x = atoi(value);
            value = g_match_info_fetch_named(match_info, "top");
            if ( value )
                y = atoi(value);
            gtk_window_move(win, x, y);
            //!!gtk_window_set_default_geometry(win, width, height);
        }
    }*/
}
gboolean sad_func(GtkTreeModel *model,
                               gint column,
                               const gchar *key,
                               GtkTreeIter *iter,
                               gpointer search_data)
{
    gchar **values = search_data;
    int count;
    gchar *key_ = g_utf8_strup(key,strlen(key));
    int len = strlen(key_);

    for ( count = 0; values[count]; count++ );

    int l = 0, h = count - 1, i, c;
    int result = 0;
    while ( l <= h ) {
        i = (l + h) >> 1;
        if ( i > count - 1 )
           i = count;
        c = strncmp(values[i], key_, len);
        if ( c < 0 ) l = i + 1;
        else {
            h = i - 1;
            if ( !c ) {
                while ( i > 0 && !strncmp(values[i], key_, len) ) i--;
                l = i;
                result = 1;
            }
        }
    }
    if ( result ) {
        GtkTreePath *path;
        path = gtk_tree_path_new_from_indices(l,-1);
        gtk_tree_model_get_iter(model, iter, path);
        gtk_tree_path_free(path);
    }
    g_free(key_);
    return FALSE;
}
GtkWidget *price_view_new(const gchar *conn_str, const gchar *firm_nick)
{
    GtkWidget *view;
    GtkTreeModel *model;
    GtkTreeViewColumn   *column;
    GtkCellRenderer     *renderer;
    GHashTable *column_names;
    int i,width;
    gchar *sname;
    gchar **names;

    model = price_model_new(conn_str,firm_nick,&names);
    view = gtk_tree_view_new_with_model(model);
    gtk_tree_view_set_fixed_height_mode(GTK_TREE_VIEW(view), TRUE);
    //gtk_tree_view_set_enable_search(GTK_TREE_VIEW(view), FALSE);
    gtk_tree_view_set_search_equal_func(GTK_TREE_VIEW(view), sad_func, names, NULL);
    gtk_tree_view_set_search_column(GTK_TREE_VIEW(view), 3);//!!


    column_names = g_hash_table_new(g_str_hash,g_str_equal);

    for ( i = 0; i < G_N_ELEMENTS(fields); i++ ) {
        if ( !fields[i].visible ) continue;
        column = gtk_tree_view_column_new();
        g_object_set_data(G_OBJECT(column), "name", fields[i].name);
        gtk_tree_view_column_set_title(column, fields[i].title);
        renderer = gtk_cell_renderer_text_new ();
        gtk_tree_view_column_pack_start (column, renderer, TRUE);
        gtk_tree_view_column_set_reorderable(column, TRUE);
        gtk_tree_view_column_set_resizable(column, TRUE);
        gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);

        //gtk_tree_view_column_set_visible(col, priv->flags & DO_PRODUCT_VIEW_FLAGS_SHOW_RECEIVED);
        if ( fields[i].func )
            gtk_tree_view_column_set_cell_data_func(column, renderer, (GtkTreeCellDataFunc)fields[i].func, GINT_TO_POINTER(i), NULL);
        gtk_tree_view_column_add_attribute (column, renderer, "text", i);

        g_object_set (renderer, "xalign", fields[i].xalign, NULL);
        g_hash_table_insert(column_names, fields[i].name, column);
        sname = g_strdup_printf("column-%s-width", fields[i].name);
        width = g_key_file_get_integer(settings, "PriceView", sname, NULL);
        if ( width )
            gtk_tree_view_column_set_fixed_width(column, width);
        g_free(sname);

    }
    gchar *value, **cols;
    value = g_key_file_get_string(settings,"PriceView","columns",NULL);
    if ( !value )
        value = g_strdup(DEFAULT_COLUMNS);
    cols=g_strsplit(value, ",", -1);
    g_free(value);
    for ( i = 0; cols[i]; i++ ) {
        column = g_hash_table_lookup(column_names, cols[i]);
        gtk_tree_view_append_column(GTK_TREE_VIEW(view), column);
    }
	/*g_signal_connect(view, "key-press-event",
				 G_CALLBACK (price_view_key_press_event_cb), priv);
	g_signal_connect(view, "destroy",
				 G_CALLBACK (price_view_destroy_cb), priv);*/
    return view;
}
GtkWidget *price_widget_new(const gchar *conn_str, const gchar *firm_nick)
{
    GtkWidget *box;
    GtkWidget *scroll;
    GtkWidget *view;
    GtkWidget *entry;
#if GTK_MAJOR_VERSION > 2
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
#else
    box = gtk_vbox_new(FALSE, 6);
#endif
    entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(box), entry, FALSE, FALSE, 0);

    scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scroll), GTK_SHADOW_IN);
    //gtk_scrolled_window_set_kinetic_scrolling(GTK_SCROLLED_WINDOW(scroll), FALSE);

    view = price_view_new(conn_str, firm_nick);
    //gtk_tree_view_set_search_entry(GTK_TREE_VIEW(view), entry);

    gtk_container_add(GTK_CONTAINER(scroll), view);

    gtk_box_pack_start(GTK_BOX(box), scroll, TRUE, TRUE, 0);

    return box;
}
