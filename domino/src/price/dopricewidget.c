/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * do-price-widget.c
 * Copyright (C) 2015 Ришес Юрий <mir@glekar.com>
 *
 * do-price-widget.c is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * do-price-widget.c is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "dopricewidget.h"
#include "dotreeview.h"
#include "../../misc/gtktreecelldatafunc.h"
#include "config.h"
#include "dosettings.h"
#include <libpq-fe.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_COLUMNS "error,order,product_name,price,stock,lifetime,supplier_name,manufactor_name"
#define BLINK_TIMEOUT 500
#define CHECKUPDATE_TIMEOUT 1000*60*10

typedef struct _DoPriceField DoPriceField;
typedef struct _DoSupplierRecord DoSupplierRecord;
typedef struct _DoPriceRecord DoPriceRecord;
typedef struct _DoPriceModel DoPriceModel;
typedef enum _DoPriceState DoPriceState;
//typedef enum _DoPriceRecordSorted DoPriceRecordSorted;

enum _DoPriceState
{
    DO_PRICE_STATE_NORMAL = 0,
    DO_PRICE_STATE_UPDATED,
    DO_PRICE_STATE_OUTDATED,
};

struct _DoPriceWidgetPrivate
{
    GtkWidget     *view;
    DoPriceModel  *model;
    GtkWidget     *view_filtered;
    GtkListStore  *model_filtered;
    GtkWidget     *paned;
    gchar         *connection_str;
    gchar         *firm_nick;
    DoPriceRecord *selected_record;
    gint           timeout_for_filtered;
    guint          filtered_timeout;

    GHashTable    *suppliers;
    GHashTable    *goods;
    GArray        *prices;

    PGconn         *conn;

    gboolean       not_linked;
    gboolean       not_lexed;

    gboolean       background_set;
    GtkWidget     *header;

    GtkWidget     *button_icons[3];
    GtkWidget     *button_supplier;
    DoPriceState   price_state;
    DoPriceState   blink_state;
    guint          blink_timeout;

    GtkWidget     *button_search;
    GtkWidget     *button_menu;

    GtkWidget     *suppliers_widget;
    GtkWidget     *suppliers_view;
    GtkListStore  *suppliers_model;

    gboolean      prices_loaded;
    gint          firm_code;

    guint          checkupdate_timeout;

};
//static const GdkRGBA  alert_color = {0.960784314,0.472745098,0,1};
static const GdkRGBA  alert_color = {1, 0.933333333,0.933333333,1};

enum
{
	PROP_0,

    PROP_CONNECTION_STR,
	PROP_FIRM_NICK1,
	PROP_HEADER_WIDGET,
};

enum  _DoPriceRecordSorted
{
    DO_PRICE_RECORD_SORTED_NONE = 0,
    DO_PRICE_RECORD_SORTED_PRICE,
    DO_PRICE_RECORD_SORTED_NAME,
};

struct _DoPriceField {
    gchar *name;
    GType type;
    gboolean visible;
    gchar *title;
    GtkTreeCellDataFunc func;
    gfloat xalign;
    gchar *default_text;
};

struct _DoSupplierRecord {
    gint        code;
    gchar      *name;
    gint        index;
    GDateTime  *time;

    gint        crntindex;
    GDateTime  *crnttime;

    GHashTable *products;

    gboolean   enabled;

};
static void do_supplier_record_clear_func(gpointer data)
{

    g_free (((DoSupplierRecord*)data)->name);
    //!!g_hash_table_foreach (((DoSupplierRecord*)data)->products, (GHFunc)g_list_free, NULL);
    g_hash_table_destroy (((DoSupplierRecord*)data)->products);
    if ( ((DoSupplierRecord*)data)->time )
        g_date_time_unref (((DoSupplierRecord*)data)->time);
    if ( ((DoSupplierRecord*)data)->crnttime )
        g_date_time_unref (((DoSupplierRecord*)data)->crnttime);
    g_free (data);
}
struct _DoPriceRecord
{
    gint      indx1;
    gint      uniq;

    gchar    *product_code;
    gint      manufactor_code;
    GDate    *lifetime;
    gchar    *product_name;
    gchar    *manufactor_name;
    gdouble   price;
    gint      stock;
    gint      min;
    gint      parcel;
    gchar    *supplier_name;
    gint      supplier_code;
    gint      order;

    gint      goods_code;

    DoSupplierRecord *owner;

    gchar      *search_name;
};

static const DoPriceField fields[] =
{
  {"product_code",G_TYPE_STRING,FALSE,"",NULL,0,NULL},
  {"manufactor_code",G_TYPE_INT,FALSE,"",NULL,0,NULL},
  {"lifetime",G_TYPE_UINT, TRUE,"Срок",gtk_tree_cell_data_func_julian_lifetime,0.5,"май 2020"},
  {"product_name",G_TYPE_STRING,TRUE,"Наименование",NULL,0,"ВИШИ ТЕРМАЛЬ АКВАЛИЯ НАБОР КРЕМ НАСЫЩЕННЫЙ 50МЛ+ГИДРОГЕЛЬ+ОЧИЩ ПЕНКА"},
  {"manufactor_name",G_TYPE_STRING,TRUE,"Производитель",NULL,0,"ВИШИ ТЕРМАЛЬ"},
  {"price",G_TYPE_DOUBLE,TRUE,"Цена",gtk_tree_cell_data_func_money,1,"1 000.23"},
  {"stock",G_TYPE_INT,TRUE,"Остаток",gtk_tree_cell_data_func_rest_int,0.5,"1000"},
  {"min",G_TYPE_INT,FALSE,"",NULL,0,NULL},
  {"parcel",G_TYPE_INT,FALSE,"",NULL,0,NULL},
  {"supplier_name",G_TYPE_STRING,TRUE,"Поставщик",NULL,0,"СИА"},
  {"supplier_code",G_TYPE_INT,FALSE,"",NULL,0,NULL},
  {"order",G_TYPE_INT,TRUE,"Заказ",gtk_tree_cell_data_func_rest_int,0.5,"90"},
  {"goods_code",G_TYPE_INT,FALSE,"",NULL,0,NULL},
  {"sad_set",G_TYPE_BOOLEAN,FALSE,"",NULL,0,NULL},
  {"underline_set",G_TYPE_BOOLEAN,FALSE,"",NULL,0,NULL},
  {"foregroupd_set",G_TYPE_BOOLEAN,FALSE,"",NULL,0,NULL},
  {"error",G_TYPE_BOOLEAN,TRUE," ",gtk_tree_cell_data_func_pixbuf_error ,0," "},
};

static gboolean do_price_widget_fill_model (DoPriceWidget *widget);
static GtkWidget *do_price_widget_make_view (DoPriceWidget *widget, const gchar *name,
                        gboolean underline,
                        gboolean foreground,
                        DoTreeViewSearchFunc search_func);
static GtkTreePath* do_price_model_search_func (DoPriceModel            *model,
						gint                     column,
						const gchar             *key,
						gpointer                 search_data);
static DoPriceModel *do_price_model_new ();
static void do_price_model_clear (DoPriceModel *model);
static gboolean do_price_model_allocate (DoPriceModel *model, gint n_row);
static gboolean do_price_model_set (DoPriceModel *model, gint row, DoPriceRecord *record);
static gboolean do_price_model_filter_set_supplier_visible (DoPriceModel *model, gint supplier_code, gboolean visible);

static gboolean do_price_widget_paned_sync_position_cb (GtkWidget *paned, GParamSpec *pspec, DoPriceWidget *widget);

static void do_price_widget_view_cursor_changed_cb (GtkTreeView *view, DoPriceWidget *widget);
static void do_price_widget_view_row_activated_cb (GtkTreeView       *tree_view,
               GtkTreePath       *path,
               GtkTreeViewColumn *column,
               DoPriceWidget *widget);

static gboolean do_price_widget_not_in_day (GDateTime *time);

static void do_price_widget_model_filtered_fill_by_record (DoPriceWidget *widget, DoPriceRecord *record);

static GList *do_price_widget_find_records_by_supplier_product (DoPriceWidget *widget, gint supplier_code, const gchar *product_code);
static GList *do_price_widget_find_records_by_goods (DoPriceWidget *widget, gint goods_code);
static void  do_price_widget_make_header (DoPriceWidget *widget);
static void do_price_widget_blink_button_supplier (DoPriceWidget *widget, gboolean blink);
static GtkWidget *do_price_widget_make_suppliers_view (DoPriceWidget *widget);
static void do_price_widget_fill_suppliers_model (DoPriceWidget *widget);
static void do_price_widget_supplier_toggled_cb (GtkCellRendererToggle *cell_renderer,
                                                  gchar                 *path,
                                                  DoPriceWidget          *widget);
static gboolean do_price_widget_check_update_prices (DoPriceWidget *widget);

G_DEFINE_TYPE (DoPriceWidget, do_price_widget, GTK_TYPE_BOX);

static void
do_price_widget_init (DoPriceWidget *do_price_widget)
{
	do_price_widget->priv = G_TYPE_INSTANCE_GET_PRIVATE (do_price_widget, DO_TYPE_PRICE_WIDGET, DoPriceWidgetPrivate);

	/* TODO: Add initialization code here */
}

GtkWidget*
do_price_widget_new (GtkWidget *Header)
{
	return g_object_new (DO_TYPE_PRICE_WIDGET,
	                      "orientation", GTK_ORIENTATION_VERTICAL,
	                      "header", Header,
	/* TODO: Add initialization code here */
	                     NULL);
}
static GObject*
do_price_widget_constructor (GType type,
                       guint n_construct_properties,
                       GObjectConstructParam *construct_params)
{
    GObject *object;
    GtkWidget *scroll;


    object = G_OBJECT_CLASS (do_price_widget_parent_class)->constructor
        (type, n_construct_properties, construct_params);


	DoPriceWidgetPrivate *priv = DO_PRICE_WIDGET (object)->priv;

    priv->background_set = TRUE;

    if ( !priv->header )
    {
        priv->header = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
        gtk_box_pack_start (GTK_BOX (object), priv->header, FALSE, FALSE, 6);

    }

    do_price_widget_make_header (DO_PRICE_WIDGET (object));

	priv->view = do_price_widget_make_view (DO_PRICE_WIDGET (object), "PriceView", FALSE, FALSE, (DoTreeViewSearchFunc)do_price_model_search_func);


	g_signal_connect (priv->view, "cursor-changed", G_CALLBACK (do_price_widget_view_cursor_changed_cb), object);
	g_signal_connect (priv->view, "row-activated", G_CALLBACK (do_price_widget_view_row_activated_cb), object);


    priv->model = do_price_model_new ();
	gtk_tree_view_set_model (GTK_TREE_VIEW (priv->view), GTK_TREE_MODEL (priv->model));

    scroll = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scroll), GTK_SHADOW_IN);
    gtk_container_add (GTK_CONTAINER (scroll), priv->view);


    priv->paned = gtk_paned_new (GTK_ORIENTATION_VERTICAL);
    gtk_paned_add1 (GTK_PANED (priv->paned), scroll);

	priv->view_filtered = do_price_widget_make_view (DO_PRICE_WIDGET (object), "PriceViewFiltered", TRUE, TRUE, NULL);
    GType types[G_N_ELEMENTS(fields)];
    gint i;
    for ( i = 0; i < G_N_ELEMENTS(fields); i++)
        types[i] = fields[i].type;
    priv->model_filtered = gtk_list_store_newv (G_N_ELEMENTS (fields), (GType*)types);
	gtk_tree_view_set_model (GTK_TREE_VIEW (priv->view_filtered), GTK_TREE_MODEL (priv->model_filtered));

    scroll = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scroll), GTK_SHADOW_IN);
    gtk_container_add (GTK_CONTAINER (scroll), priv->view_filtered);

    gtk_paned_add2 (GTK_PANED (priv->paned), scroll);


    gtk_box_pack_start (GTK_BOX (object), priv->paned, TRUE, TRUE, 0);
    gtk_paned_set_position (GTK_PANED (priv->paned), do_settings_get_int ("PriceWidget", "paned-position", 200));

    g_signal_connect (priv->paned, "notify::position",
                     G_CALLBACK (do_price_widget_paned_sync_position_cb), object);


    do_price_widget_fill_model (DO_PRICE_WIDGET (object));

    priv->timeout_for_filtered = do_settings_get_int ("PriceWidget", "filtered-timeout", 500);


    //g_idle_add (do_price_widget_grab_focus, object);
    do_tree_view_grab_focus (DO_TREE_VIEW (priv->view));

    priv->checkupdate_timeout =
        g_timeout_add (CHECKUPDATE_TIMEOUT,
		   (GSourceFunc) do_price_widget_check_update_prices, object);




	return object;
}

static void
do_price_widget_finalize (GObject *object)
{
	DoPriceWidgetPrivate *priv = DO_PRICE_WIDGET (object)->priv;
	/* TODO: Add deinitalization code here */
	g_free (priv->connection_str);
    g_free (priv->firm_nick);

    if ( priv->blink_timeout )
        g_source_remove (priv->blink_timeout);

    if ( priv->checkupdate_timeout )
        g_source_remove (priv->checkupdate_timeout);

    if ( priv->suppliers )
        g_hash_table_destroy (priv->suppliers);

	if ( priv->filtered_timeout != 0 )
        g_source_remove (priv->filtered_timeout);

    if ( priv->conn )
        PQfinish (priv->conn);

	G_OBJECT_CLASS (do_price_widget_parent_class)->finalize (object);
}

static void
do_price_widget_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	g_return_if_fail (DO_IS_PRICE_WIDGET (object));
	DoPriceWidgetPrivate *priv = DO_PRICE_WIDGET (object)->priv;

	switch (prop_id)
	{
	case PROP_CONNECTION_STR:
        g_free (priv->connection_str);
        priv->connection_str = g_value_dup_string (value);
        do_price_widget_fill_model (DO_PRICE_WIDGET (object));
		break;
	case PROP_FIRM_NICK1:
        g_free (priv->firm_nick);
        priv->firm_nick = g_value_dup_string (value);
        do_price_widget_fill_model (DO_PRICE_WIDGET (object));
		break;
	case PROP_HEADER_WIDGET:
        priv->header = g_value_get_object (value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
do_price_widget_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	g_return_if_fail (DO_IS_PRICE_WIDGET (object));
	DoPriceWidgetPrivate *priv = DO_PRICE_WIDGET (object)->priv;

	switch (prop_id)
	{
	case PROP_CONNECTION_STR:
		g_value_set_string (value, priv->connection_str);
		break;
	case PROP_FIRM_NICK1:
		g_value_set_string (value, priv->firm_nick);
		break;
	case PROP_HEADER_WIDGET:
		g_value_set_object (value, priv->header);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
do_price_widget_class_init (DoPriceWidgetClass *klass)
{
	GObjectClass* object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (DoPriceWidgetPrivate));

	object_class->constructor = do_price_widget_constructor;
	object_class->finalize = do_price_widget_finalize;
	object_class->set_property = do_price_widget_set_property;
	object_class->get_property = do_price_widget_get_property;

	g_object_class_install_property (object_class,
	                                 PROP_CONNECTION_STR,
	                                 g_param_spec_string ("connection_str",
	                                                      "connection-str",
	                                                      "connection string",
	                                                      "host=localhost dbname=domino user=domino",
	                                                      G_PARAM_READABLE | G_PARAM_WRITABLE));

	g_object_class_install_property (object_class,
	                                 PROP_FIRM_NICK1,
	                                 g_param_spec_string ("firm_nick",
	                                                      "firm-nick",
	                                                      "firm nick",
	                                                      "",
	                                                      G_PARAM_READABLE | G_PARAM_WRITABLE));
	g_object_class_install_property (object_class,
	                                 PROP_HEADER_WIDGET,
	                                 g_param_spec_object("header",
	                                                      "header",
	                                                      "header",
	                                                      GTK_TYPE_CONTAINER,
	                                                      G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
}
static void
do_price_widget_check_outdated_price (gpointer       key,
                                      gpointer       value,
                                      gpointer       user_data)
{
    DoSupplierRecord *rec = value;
    DoPriceWidget *widget = user_data;

    if ( rec->enabled  )
    {
        if ( do_price_widget_not_in_day (rec->time) )
        {
            widget->priv->price_state = DO_PRICE_STATE_OUTDATED;
            return;
        }
    }
    if ( rec->crnttime &&
         ( !rec->time ||
            g_date_time_difference(rec->crnttime, rec->time) > 0 ) )
    {
        if ( widget->priv->price_state != DO_PRICE_STATE_OUTDATED )
            widget->priv->price_state = DO_PRICE_STATE_UPDATED;
        return;
    }
}
static void
do_price_widget_check_outdated (DoPriceWidget *widget)
{
    widget->priv->price_state = DO_PRICE_STATE_NORMAL;
    g_hash_table_foreach (widget->priv->suppliers, do_price_widget_check_outdated_price, widget);
    g_object_ref (widget->priv->button_icons[widget->priv->price_state]);
    gtk_button_set_image (GTK_BUTTON (widget->priv->button_supplier), widget->priv->button_icons[widget->priv->price_state]);
    do_price_widget_blink_button_supplier (widget, widget->priv->price_state != DO_PRICE_STATE_NORMAL);
}
static void
do_price_widget_supplier_get_last_prices (gint *code, DoSupplierRecord *rec, DoPriceWidget *widget)
{
    DoPriceWidgetPrivate *priv = widget->priv;

    PGresult *res;
    gchar *sql;
    gint index = 0;
    GDateTime *time;

    if ( !priv->conn || !priv->firm_code )
        return;

    time = NULL;
    sql = g_strdup_printf("SELECT price_index,price_time FROM _price"
                          " WHERE supplier_code=%d AND firm_code=%d"
                          " ORDER BY price_time DESC, price_index DESC LIMIT 1" ,
                          rec->code, priv->firm_code);
    res = PQexec(priv->conn, sql);
    if ( PQresultStatus(res) != PGRES_TUPLES_OK )
    {
        ERROR("%s", PQerrorMessage(priv->conn));
        return;
    }
    if ( PQntuples(res) ) {
        gchar **val,**val1,**val2;
        index = atoi(PQgetvalue(res, 0, 0));
        val = g_strsplit(PQgetvalue(res, 0, 1)," ",-1);
        if ( g_strv_length(val) == 2)
        {
            val1 = g_strsplit(val[0],"-",-1);
            val2 = g_strsplit(val[1],":",-1);
            time = g_date_time_new_local (atoi(val1[0]),atoi(val1[1]),atoi(val1[2]),
                                        atoi(val2[0]),atoi(val2[1]),atoi(val2[2]));
            g_strfreev(val1);
            g_strfreev(val2);
        }
        g_strfreev(val);
    }
    PQclear(res);
    g_free(sql);
    if ( priv->prices_loaded )
    {
        if ( rec->crnttime )
            g_date_time_unref (rec->crnttime);
        rec->crnttime = time;
        rec->crntindex = index;
    }
    else
    {
        if ( rec->time )
            g_date_time_unref (rec->time);
        rec->time = time;
        rec->index = index;
    }
}
static gboolean
do_price_widget_fill_model (DoPriceWidget *widget)
{

	DoPriceWidgetPrivate *priv = widget->priv;

	if ( !priv->firm_nick || !priv->connection_str )
        return FALSE;

    DoPriceModel *model = priv->model;
    int i;

    MESSAGE_WITH_TIME("start read db");

    priv->conn = PQconnectdb(priv->connection_str);
    if ( PQstatus(priv->conn) != CONNECTION_OK )
    {
        ERROR("Connect to '%s'. %s", priv->connection_str, PQerrorMessage(priv->conn));
        priv->conn = NULL;
        return FALSE;
    }

    PGresult *res;
    int nrow;
    GHashTable *price_index;
    DoSupplierRecord *rec;
    gchar *sql,*buf;
    gchar *str;
    gint index;
    GList *list, *l;

    /*
    res = PQexec(priv->conn, "SELECT code,nick FROM get_firms() WHERE nick <> ''");

    if ( PQresultStatus(res) != PGRES_TUPLES_OK )
    {
        ERROR("%s", PQerrorMessage(priv->conn));
        return FALSE;
    }

    nrow = PQntuples(res);
    for ( i = 0; i < nrow; i++ ) {
        buf = PQgetvalue(res, i, 1);
        if ( !g_strcmp0(buf, priv->firm_nick) ) {
            priv->firm_code = atoi(PQgetvalue(res, i, 0));
            break;
        }
    }
    PQclear(res);
    res = PQexec(priv->conn, "SELECT code,name FROM get_suppliers() WHERE nick <> ''");
    if ( PQresultStatus(res) != PGRES_TUPLES_OK )
    {
        ERROR("%s", PQerrorMessage(priv->conn));
        return FALSE;
    }

    str = NULL;
    price_index = g_hash_table_new(g_int_hash,g_int_equal);
    priv->suppliers = g_hash_table_new_full (g_int_hash, g_int_equal, NULL, do_supplier_record_clear_func);
    priv->goods = g_hash_table_new_full (g_int_hash, g_int_equal, NULL, NULL); //!!

    nrow = PQntuples(res);
    for ( i = 0; i < nrow; i++ ) {
        rec = g_new0 (DoSupplierRecord, 1);
        rec->code = atoi(PQgetvalue(res, i, 0));
        rec->name = g_strdup(PQgetvalue(res, i, 1));
        rec->products = g_hash_table_new(g_str_hash, g_str_equal);
        rec->enabled = TRUE;//!!

        g_hash_table_insert (priv->suppliers, &rec->code, rec);
    }
    PQclear(res);
    g_hash_table_foreach (priv->suppliers, (GHFunc)do_price_widget_supplier_get_last_prices, widget);
    list = g_hash_table_get_values (priv->suppliers);
    for ( l = list; l; l=l->next )
    {
        rec = l->data;
        if ( rec->index )
        {
            if ( str )
                { buf = g_strdup_printf("%s,%d", str, rec->index);g_free(str);str=buf;}
            else
                str=g_strdup_printf("%d", rec->index);
            g_hash_table_insert(price_index, &rec->index, rec);
        }
    }
    g_list_free (list);


    do_price_widget_check_outdated (widget);
    */
    time_t t1 = time(NULL);

    MESSAGE("read");

    sql = g_strdup_printf(
                "SELECT"
				"  goods_code,manufactor_code,lifetime,goods_name,"
				"  manufactor_name,"
				"  price,stock,quant_min,quant_parcel,price_index,g_code"
				" FROM price_position WHERE price_index in (%s)"
				" ORDER BY lower(goods_name),manufactor_name,lifetime DESC",str);
    res = PQexec(priv->conn, sql);
    if ( PQresultStatus(res) != PGRES_TUPLES_OK )
    {
        ERROR("Connect to '%s'. %s", priv->connection_str, PQerrorMessage(priv->conn));
        return FALSE;
    }
    nrow = PQntuples(res);
    do_price_model_clear (model);
    do_price_model_allocate (model, nrow);
    for ( i = 0; i < nrow; i++ ) {
        DoSupplierRecord *supplier;
        DoPriceRecord *record;
        gchar *str;
        gchar **val;


        record = g_new0 (DoPriceRecord, 1);

        record->product_code = g_strdup (PQgetvalue(res, i, 0));
        record->manufactor_code = atoi (PQgetvalue(res, i, 1));

        str = g_utf8_normalize (PQgetvalue (res, i, 3), -1, G_NORMALIZE_ALL);
        record->search_name = g_utf8_casefold (str, -1);
        g_free (str);

        val = g_strsplit(PQgetvalue(res, i, 2),"-",-1);
        if ( g_strv_length(val) == 3)
            record->lifetime = g_date_new_dmy(atoi(val[2]),atoi(val[1]),atoi(val[0]));
        g_strfreev(val);
        record->product_name = g_strdup (PQgetvalue(res, i, 3));
        record->manufactor_name = g_strdup (PQgetvalue(res, i, 4));
        record->price = atof(PQgetvalue(res, i, 5));
        record->stock = atoi (PQgetvalue(res, i, 6));
        record->min =  atoi (PQgetvalue(res, i, 7));
        record->parcel =  atoi (PQgetvalue(res, i, 8));
        index = atoi(PQgetvalue(res, i, 9));
        supplier = g_hash_table_lookup(price_index, &index);
        if ( supplier )
        {
            GList *list;
            record->supplier_name = supplier->name;
            record->supplier_code = supplier->code;
            list = g_hash_table_lookup (supplier->products, record->product_code);
            g_hash_table_insert (supplier->products, record->product_code, g_list_append (list, record));
            record->owner = supplier;
        }
        else
        {
            g_assert (supplier != NULL);
        }
        //record->order = 0;
        record->goods_code = atoi (PQgetvalue(res, i, 10));
        do_price_model_set (model, i, record);
        list = g_hash_table_lookup (priv->goods, &record->goods_code);
        g_hash_table_insert (priv->goods, &record->goods_code, g_list_append (list, record));

    }
    PQclear(res);
    g_free(sql);
    time_t t2 = time(NULL);
    MESSAGE("sec %d",(int)(t2-t1));
    priv->prices_loaded = TRUE;

    do_price_widget_fill_suppliers_model (widget);

    return TRUE;
}

static GtkWidget*
do_price_widget_make_view (DoPriceWidget *widget, const gchar *name, gboolean underline, gboolean foreground, DoTreeViewSearchFunc search_func)
{
    //DoPriceWidgetPrivate *priv = widget->priv;

    GtkWidget *view;
    GtkTreeViewColumn   *column;
    GtkCellRenderer     *renderer;
    int i;
    GList *list_=NULL, *l;

    view = do_tree_view_new (name);
    gtk_tree_view_set_fixed_height_mode (GTK_TREE_VIEW (view), TRUE);
    gtk_tree_view_set_enable_search (GTK_TREE_VIEW (view), TRUE);
    //do_tree_view_set_search_func (GTK_TREE_VIEW (view), sad_func, names, NULL);
    //gtk_tree_view_set_search_column(GTK_TREE_VIEW(view), 3);//!!
    if ( search_func )
        do_tree_view_set_search_func (DO_TREE_VIEW (view), search_func, widget, NULL);
    //gtk_tree_view_set_hover_selection (GTK_TREE_VIEW (view), FALSE);


    for ( i = 0; i < G_N_ELEMENTS(fields); i++ )
    {
        if ( !fields[i].visible ) continue;
        column = do_tree_view_column_new(DO_TREE_VIEW (view), fields[i].name, fields[i].title, fields[i].default_text);
        gtk_tree_view_column_set_reorderable (column, TRUE);

        if ( !strcmp (fields[i].name,"error") )
        {
            renderer = gtk_cell_renderer_pixbuf_new();
            gtk_tree_view_column_pack_start (column, renderer, TRUE);
        }
        else
        {
            renderer = gtk_cell_renderer_text_new ();
            gtk_tree_view_column_pack_start (column, renderer, TRUE);
            gtk_tree_view_column_set_reorderable(column, TRUE);
            gtk_tree_view_column_add_attribute (column, renderer, "text", i);
        }


        list_ = g_list_append (list_, column);

        //gtk_tree_view_column_set_visible(col, priv->flags & DO_PRODUCT_VIEW_FLAGS_SHOW_RECEIVED);
        if ( fields[i].func )
            gtk_tree_view_column_set_cell_data_func(column, renderer, (GtkTreeCellDataFunc)fields[i].func, GINT_TO_POINTER(i), NULL);

        if ( underline && (!strcmp (fields[i].name,"product_name")) )
        {
            gtk_tree_view_column_add_attribute (column, renderer, "underline-set", 14);
            g_object_set (renderer, "underline", PANGO_UNDERLINE_SINGLE, NULL);
        }
        if ( foreground && strcmp (fields[i].name,"error"))
        {
            gtk_tree_view_column_add_attribute (column, renderer, "foreground-set", 15);
            g_object_set (renderer, "foreground", "green", NULL);
        }
        if ( widget->priv->background_set && strcmp (fields[i].name,"error"))
        {
            gtk_tree_view_column_add_attribute (column, renderer, "background-set", 16);
            g_object_set (renderer, "background-rgba", &alert_color, NULL);
            //g_object_set (renderer, "background", "magenta", NULL);
        }

        g_object_set (renderer, "xalign", fields[i].xalign, NULL);
    }
    gchar *value, **cols;
    value = do_settings_get_string (name, "columns", DEFAULT_COLUMNS);
    if ( !value[0] )
    {
        g_free (value);
        value = g_strdup (DEFAULT_COLUMNS);
    }
    cols=g_strsplit(value, ",", -1);
    g_free(value);
    for ( i = 0; cols[i]; i++ )
    {
        for ( l = list_; l; l=l->next )
        {
            gchar *name;
            name = (char*)g_object_get_data (G_OBJECT (l->data), "name");
            if ( !g_strcmp0 (name, cols[i]) )
            {
                do_tree_view_append_column (DO_TREE_VIEW (view), l->data);
                if ( !g_strcmp0 ("product_name", cols[i]) )
                {
                    GList *list;
                    list = gtk_cell_layout_get_cells (GTK_CELL_LAYOUT (l->data));
                    do_tree_view_set_search_inline_column (DO_TREE_VIEW (view),
                                                           GTK_TREE_VIEW_COLUMN (l->data),
                                                           GTK_CELL_RENDERER (list->data));
                    g_list_free (list);
                }
                break;
            }
        }
    }
    gtk_tree_view_set_search_column (GTK_TREE_VIEW (view), 3);
    return view;
}

static void
do_price_record_free (DoPriceRecord *record)
{
    g_free (record->supplier_name);
    g_date_free (record->lifetime);
    g_free (record->manufactor_name);
    g_free (record->product_name);
    g_free (record->search_name);
    g_free (record->product_code);
    g_free (record);
}

typedef struct _DoPriceModelClass DoPriceModelClass;

#define DO_PRICE_MODEL_TYPE                  (do_price_model_get_type ())
#define DO_PRICE_MODEL(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), DO_PRICE_MODEL_TYPE, DoPriceModel))
#define DO_PRICE_MODEL_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass),  DO_PRICE_MODEL_TYPE, DoPriceModelClass))
#define DO_IS_PRICE_MODEL(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), DO_PRICE_MODEL_TYPE))
#define DO_IS_PRICE_MODEL_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass),  DO_PRICE_MODEL_TYPE))
#define DO_PRICE_MODEL_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj),  DO_PRICE_MODEL_TYPE, DoPriceModelClass))

struct _DoPriceModel
{
    GObject              parent;      /* this MUST be the first member */
    DoPriceRecord  **records_all;
    DoPriceRecord  **records_filtered;
    gint             n_record_all;
    gint             n_record_filtered;
    gint             stamp;
};
struct _DoPriceModelClass
{
    GObjectClass parent_class;
    //!!void            (* refresh_record)  (DoProductModel *model, GtkTreeIter *iter, gpointer data);
};
static void do_price_model_tree_model_init (GtkTreeModelIface *iface);
static void do_price_model_init(DoPriceModel *pkg_tree);
static void do_price_model_class_init(DoPriceModelClass *klass);
static void do_price_model_tree_model_init(GtkTreeModelIface *iface);
static void do_price_model_finalize(GObject *object);
static GtkTreeModelFlags do_price_model_get_flags(GtkTreeModel *tree_model);
static gint do_price_model_get_n_columns(GtkTreeModel *tree_model);
static GType do_price_model_get_column_type(GtkTreeModel *tree_model, gint index);
static gboolean do_price_model_get_iter(GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreePath *path);
static GtkTreePath *do_price_model_get_path(GtkTreeModel *tree_model, GtkTreeIter *iter);
static void do_price_model_get_value(GtkTreeModel *tree_model, GtkTreeIter *iter, gint column, GValue *value);
static gboolean do_price_model_iter_next(GtkTreeModel *tree_model, GtkTreeIter *iter);
static gboolean do_price_model_iter_children(GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *parent);
static gboolean do_price_model_iter_has_child(GtkTreeModel *tree_model, GtkTreeIter *iter);
static gint do_price_model_iter_n_children(GtkTreeModel *tree_model, GtkTreeIter *iter);
static gboolean do_price_model_iter_nth_child(GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *parent, gint n);
static gboolean do_price_model_iter_parent(GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *child);


G_DEFINE_TYPE_WITH_CODE (DoPriceModel, do_price_model, G_TYPE_OBJECT,
			 G_IMPLEMENT_INTERFACE (GTK_TYPE_TREE_MODEL,
						do_price_model_tree_model_init));

static void
do_price_model_tree_model_init (GtkTreeModelIface *iface)
{
    iface->get_flags       = do_price_model_get_flags;
    iface->get_n_columns   = do_price_model_get_n_columns;
    iface->get_column_type = do_price_model_get_column_type;
    iface->get_iter        = do_price_model_get_iter;
    iface->get_path        = do_price_model_get_path;
    iface->get_value       = do_price_model_get_value;
    iface->iter_next       = do_price_model_iter_next;
    iface->iter_children   = do_price_model_iter_children;
    iface->iter_has_child  = do_price_model_iter_has_child;
    iface->iter_n_children = do_price_model_iter_n_children;
    iface->iter_nth_child  = do_price_model_iter_nth_child;
    iface->iter_parent     = do_price_model_iter_parent;
}

static DoPriceModel*
do_price_model_new ()
{
    return (DoPriceModel*)g_object_new (DO_PRICE_MODEL_TYPE,
                                    NULL);
}
static void
do_price_model_clear (DoPriceModel *model)
{
    gint i;
    GtkTreePath *path;
    path = gtk_tree_path_new ();
    for ( i = 0; i < model->n_record_all; i++ )
    {
        gtk_tree_path_append_index (path, i);
        gtk_tree_model_row_deleted (GTK_TREE_MODEL (model), path);
        do_price_record_free (model->records_all[i]);
        model->records_all[i] = NULL;
    }
    gtk_tree_path_free (path);
    model->n_record_all = 0;
    model->n_record_filtered = 0;
    g_free (model->records_all);
    g_free (model->records_filtered);
    model->records_all = model->records_filtered = NULL;
}
static gboolean
do_price_model_allocate (DoPriceModel *model, gint n_row)
{
    if ( model->n_record_all )
        do_price_model_clear (model);
    if ( !n_row ) return TRUE;

    model->records_all = g_new0 (DoPriceRecord*, n_row);
    model->records_filtered = g_new0 (DoPriceRecord*, n_row);
    if ( model->records_all )
    {
        model->n_record_all = n_row;
        model->n_record_filtered = n_row; //!!
    }
    return model->records_all != NULL;
}
static gboolean
do_price_model_set (DoPriceModel *model, gint row, DoPriceRecord *record)
{
    g_return_val_if_fail (row >= 0  || row < model->n_record_all, FALSE);
    g_return_val_if_fail (record != NULL, FALSE);

    gboolean new_;
    GtkTreePath *path;
    GtkTreeIter iter;
    path = gtk_tree_path_new ();

    new_ = model->records_all[row] == NULL;

    if ( !new_ )
        do_price_record_free (model->records_all[row]);
    record->uniq = row;
    record->indx1 = row;
     model->records_all[row] = model->records_filtered[row] = record;

    iter.stamp = model->stamp;
    iter.user_data  = record;

    gtk_tree_path_append_index (path, row);

    if ( new_ )
        gtk_tree_model_row_inserted (GTK_TREE_MODEL (model), path, &iter);
    else
        gtk_tree_model_row_changed (GTK_TREE_MODEL (model), path, &iter);

    gtk_tree_path_free (path);

    return TRUE;
}
static void
do_price_model_init (DoPriceModel *model)
{
    model->stamp = g_random_int(); // Random int to check whether iters belong to out model

}
static GObjectClass *parent_class = NULL;  /* GObject stuff - nothing to worry about */

static void
do_price_model_class_init(DoPriceModelClass *klass)
{
    GObjectClass *object_class;

    parent_class = (GObjectClass*) g_type_class_peek_parent (klass);
    object_class = (GObjectClass*) klass;

	//object_class->constructor  = do_price_model_constructor;
    object_class->finalize = do_price_model_finalize;

	//object_class->get_property = do_price_model_get_property;
	//object_class->set_property = do_price_model_set_property;
}
static void
do_price_model_finalize(GObject *object)
{
    do_price_model_clear (DO_PRICE_MODEL (object));
}
static GtkTreeModelFlags
do_price_model_get_flags (GtkTreeModel *tree_model)
{
    g_return_val_if_fail (DO_IS_PRICE_MODEL(tree_model), (GtkTreeModelFlags)0);

    return (GTK_TREE_MODEL_LIST_ONLY | GTK_TREE_MODEL_ITERS_PERSIST);
}
static gboolean
do_price_widget_not_in_day (GDateTime *time)
{
    if ( !time )
        return TRUE;
    GDateTime *now;
    GTimeSpan diff;
    now = g_date_time_new_now_local ();
    diff = g_date_time_difference(now, time);
    g_date_time_unref (now);
    return diff/1000000./60./60. > 24;
}

static gint
do_price_model_get_n_columns(GtkTreeModel *tree_model)
{
    return G_N_ELEMENTS(fields);
}
static GType
do_price_model_get_column_type (GtkTreeModel *tree_model, gint index)
{
    g_return_val_if_fail (DO_IS_PRICE_MODEL (tree_model), G_TYPE_INVALID);
    g_return_val_if_fail (index < G_N_ELEMENTS(fields) && index >= 0, G_TYPE_INVALID);

    return fields[index].type;
}

static gboolean
do_price_model_get_iter (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreePath *path)
{
    DoPriceRecord  *record;
    gint           *indices, n, depth;


    g_assert (DO_IS_PRICE_MODEL(tree_model));
    g_assert (path != NULL);

    indices = gtk_tree_path_get_indices (path);
    depth   = gtk_tree_path_get_depth (path);

    g_assert (depth == 1); // depth 1 = top level; a model only has top level nodes and no children

    n = indices[0]; // the n-th top level row

    if ( n >= DO_PRICE_MODEL (tree_model)->n_record_filtered || n < 0 )
        return FALSE;

    record = DO_PRICE_MODEL (tree_model)->records_filtered[n];

    g_assert (record != NULL);

    iter->stamp      = DO_PRICE_MODEL (tree_model)->stamp;
    iter->user_data  = record;
    iter->user_data2 = NULL;
    iter->user_data3 = NULL;

    return TRUE;
}
static GtkTreePath*
do_price_model_get_path (GtkTreeModel *tree_model, GtkTreeIter  *iter)
{
    GtkTreePath  *path;
    DoPriceRecord *record;

    g_return_val_if_fail (DO_IS_PRICE_MODEL(tree_model), NULL);
    g_return_val_if_fail (iter != NULL,               NULL);
    g_return_val_if_fail (iter->user_data != NULL,    NULL);

    record = (DoPriceRecord*) iter->user_data;

    path = gtk_tree_path_new();
    gtk_tree_path_append_index(path, record->indx1);

    return path;
}

static void
do_price_model_get_value (GtkTreeModel *tree_model, GtkTreeIter *iter, gint column, GValue *value)
{
    DoPriceRecord *record;

    g_return_if_fail (DO_IS_PRICE_MODEL (tree_model));
    g_return_if_fail (iter != NULL);
    g_return_if_fail (column < G_N_ELEMENTS (fields));

    g_value_init (value, fields[column].type);

    record = (DoPriceRecord*) iter->user_data;

    if ( !record ) return;

    switch (column)
    {
        case 0:
            g_value_set_string (value, record->product_code);
            break;
        case 1:
            g_value_set_int (value, record->manufactor_code);
            break;
        case 2:
            if ( record->lifetime )
                g_value_set_uint (value, g_date_get_julian(record->lifetime));
            break;
        case 3:
            g_value_set_string (value, record->product_name);
            break;
        case 4:
            g_value_set_string (value, record->manufactor_name);
            break;
        case 5:
            g_value_set_double (value, record->price);
            break;
        case 6:
            g_value_set_int (value, record->stock);
            break;
        case 7:
            g_value_set_int (value, record->min);
            break;
        case 8:
            g_value_set_int (value, record->parcel);
            break;
        case 9:
            g_value_set_string (value, record->supplier_name);
            break;
        case 10:
            g_value_set_int (value, record->supplier_code);
            break;
        case 11:
            g_value_set_int (value, record->order);
            break;
        case 12:
            g_value_set_int (value, record->goods_code);
            break;
        case 16:
            g_value_set_boolean (value, do_price_widget_not_in_day (record->owner->time));
            break;
        default:
            break;//
            //!!g_value_set_int (value, 0);
    };
};
static gboolean
do_price_model_iter_next (GtkTreeModel *tree_model, GtkTreeIter *iter)
{
    DoPriceRecord *record;

    g_return_val_if_fail (DO_IS_PRICE_MODEL (tree_model), FALSE);

    if (iter == NULL || iter->user_data == NULL)
        return FALSE;

    record = (DoPriceRecord *) iter->user_data;

    if ((record->indx1 + 1) >= DO_PRICE_MODEL (tree_model)->n_record_filtered )
        return FALSE;

    record = DO_PRICE_MODEL (tree_model)->records_filtered[record->indx1 + 1];
    iter->stamp     = DO_PRICE_MODEL (tree_model)->stamp;
    iter->user_data = record;
    return TRUE;
}
static gboolean
do_price_model_iter_children (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *parent)
{
    return FALSE;
}
static gboolean
do_price_model_iter_has_child (GtkTreeModel *tree_model, GtkTreeIter *iter)
{
    return FALSE;
}
static gint
do_price_model_iter_n_children (GtkTreeModel *tree_model, GtkTreeIter *iter)
{
    if ( !iter )
    {
        MESSAGE("count %d",DO_PRICE_MODEL (tree_model)->n_record_filtered);

        return DO_PRICE_MODEL (tree_model)->n_record_filtered;
    }
    return 0;
}
static gboolean
do_price_model_iter_nth_child (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *parent, gint n)
{
    if ( parent )
        return FALSE;

    if ( n >= DO_PRICE_MODEL (tree_model)->n_record_filtered )
            return FALSE;

    iter->stamp = DO_PRICE_MODEL (tree_model)->stamp;
    iter->user_data = DO_PRICE_MODEL (tree_model)->records_filtered[n];


    return TRUE;

}

static gboolean
do_price_model_iter_parent (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *child)
{
    return FALSE;
}
static DoPriceRecord*
do_price_model_get_record (DoPriceModel *model, GtkTreePath *path)
{
    gint           *indices, n, depth;

    g_return_val_if_fail (path != NULL, NULL);
    indices = gtk_tree_path_get_indices (path);
    depth   = gtk_tree_path_get_depth (path);
    g_return_val_if_fail (depth == 1, NULL);
    n = indices[0];

    if ( n >= model->n_record_filtered || n < 0 )
        return NULL;

    return model->records_filtered[n];

}
static gboolean
do_price_widget_paned_sync_position_cb (GtkWidget *paned, GParamSpec *pspec, DoPriceWidget *widget)
{
    do_settings_set_int ("PriceWidget", "paned-position", gtk_paned_get_position (GTK_PANED (paned)));
    return TRUE;
}
static gboolean
do_price_widget_view_filtered_timeout (DoPriceWidget *widget)
{
    widget->priv->filtered_timeout = 0;
    do_price_widget_model_filtered_fill_by_record (widget, widget->priv->selected_record);
    return FALSE;
}
static void
do_price_widget_view_cursor_changed_cb (GtkTreeView *view, DoPriceWidget *widget)
{
	DoPriceWidgetPrivate *priv = widget->priv;
	DoPriceRecord *record;


    GtkTreePath *path = NULL;
    gtk_tree_view_get_cursor (view, &path, NULL);

    if ( !path ) return;

    record = do_price_model_get_record ((DoPriceModel*)priv->model, path);
    gtk_tree_path_free (path);

    if ( !record ) return;
    if ( priv->selected_record && priv->selected_record->uniq == record->uniq ) return;

    if ( widget->priv->filtered_timeout )
        g_source_remove (widget->priv->filtered_timeout);


    priv->selected_record = record;

    priv->filtered_timeout =
        g_timeout_add (priv->timeout_for_filtered,
		   (GSourceFunc) do_price_widget_view_filtered_timeout, widget);
}
#define min(a,b) ((a < b) ? a : b)
static gint
do_price_widget_get_goods_by_lex (DoPriceWidget *widget, const gchar *lex, gint *in_goods, gint **goods)
{
    DoPriceWidgetPrivate *priv = widget->priv;
    PGresult *res;
    gint nrow,i;
    gchar *sql;

    gchar *condition;
    condition = NULL;
    if ( lex[0] == '0' && ( lex[1] == ',' || lex[1] == '.' ) )
    {
        gint n;
        n = atoi (lex+2);
        if ( n && n < 1000 )
        {
            if ( n < 10 )
                n*=100;
            else
            if ( n < 100 )
                n*=10;
            condition = g_strdup_printf ("lex in ('%s','%d')", lex, n);
        }
    }
    if ( !condition )
        condition = g_strdup_printf ("lex = '%s'", lex);
    if ( in_goods )
    {
        gchar *buf,*goods_str;
        buf = NULL;
        for ( i = 0; in_goods[i] ; i++ )
        {
            if ( !i )
                goods_str = g_strdup_printf ("%d", in_goods[i]);
            else
            {
                buf = g_strdup_printf ("%s,%d", goods_str, in_goods[i]);
                g_free (goods_str);
                goods_str = buf;
            }
        }
        buf = g_strdup_printf("%s AND code IN (%s)", condition, goods_str);
        g_free (goods_str); g_free (condition);
        condition = buf;
    }
    sql = g_strdup_printf ("SELECT code FROM _price_handbook_goods_lex"
                           " WHERE %s", condition);
    //MESSAGE("%s",sql);
    res = PQexec(priv->conn, sql);
    if ( PQresultStatus(res) != PGRES_TUPLES_OK )
    {
        ERROR("Connect to '%s'. %s", priv->connection_str, PQerrorMessage(priv->conn));
        return -1;
    }
    nrow = PQntuples(res);
    *goods = (gint*)g_new0(gint*, nrow+1);
    for ( i = 0; i < nrow; i++ )
        (*goods)[i] = atoi(PQgetvalue (res, i, 0));
    PQclear (res);
    g_free (sql);
    return nrow;
}
static gint
price_cmp (gconstpointer  a, gconstpointer  b)
{
    if ( ((DoPriceRecord*)a)->price > ((DoPriceRecord*)b)->price )
        return 1;
    if ( ((DoPriceRecord*)a)->price < ((DoPriceRecord*)b)->price )
        return -1;
    return 0;
}
static void
do_price_widget_add_rec_to_model (GtkListStore *store, GtkTreeIter *iter,  DoPriceRecord *rec)
{

    gtk_list_store_set (store, iter,
                        0, rec->product_code,
                        1, rec->manufactor_code,
                        2, (rec->lifetime != NULL) ? g_date_get_julian(rec->lifetime) : 0,
                        3, rec->product_name,
                        4, rec->manufactor_name,
                        5, rec->price,
                        6, rec->stock,
                        7, rec->min,
                        8, rec->parcel,
                        9, rec->supplier_name,
                        10,rec->supplier_code,
                        11,rec->order,
                        //14, TRUE,
                        16,do_price_widget_not_in_day (rec->owner->time),
                        -1);
}
static void
get_supplier_str (gpointer key, DoSupplierRecord *record, gchar **str_)
{
    if ( record->enabled )
    {
        if ( *str_ )
        {
            gchar *buf;
            buf = g_strdup_printf ("%s,%d",*str_,record->code);
            g_free (*str_);
            *str_ = buf;
        }
        else
            *str_ = g_strdup_printf ("%d", record->code);
    }
}
static void
do_price_widget_model_filtered_fill_by_record (DoPriceWidget *widget, DoPriceRecord *record)
{
    DoPriceWidgetPrivate *priv = widget->priv;
    PGresult *res;
    gint nrow,i,count,j,k,flag;
    gchar *sql;
    gchar **lexs;
    gint **goods;
    GList *indexes=NULL, *indx, *list, *l;
    gint *len;
    gint add_count;
    GtkTreeIter iter;

    gtk_list_store_clear (priv->model_filtered);

    if ( !priv->conn )  return;

    if  ( !priv->not_linked )
    {
        gchar *supplier_str=NULL;

        g_hash_table_foreach (priv->suppliers, (GHFunc)get_supplier_str, &supplier_str);

        g_assert (supplier_str != NULL);


        sql = g_strdup_printf ("SELECT supplier_code, product_code FROM _product_link"
                           " WHERE domino_code in "
                           " (SELECT domino_code FROM _product_link WHERE supplier_code = %d AND product_code = '%s') AND "
                           " supplier_code IN (%s)",
                            record->supplier_code, record->product_code, supplier_str);
        res = PQexec(priv->conn, sql);
        if ( PQresultStatus(res) != PGRES_TUPLES_OK )
        {
            ERROR("Connect to '%s'. %s", priv->connection_str, PQerrorMessage(priv->conn));
            return;
        }
        nrow = PQntuples(res);
        list = NULL;

        if ( nrow )
        {
            add_count = 0;
            for ( i = 0; i < nrow; i++ )
            {
                l = do_price_widget_find_records_by_supplier_product (widget, atoi(PQgetvalue(res, i, 0)), PQgetvalue(res, i, 1));
                for ( ;l; l=l->next )
                {
                    list = g_list_append (list, l->data);
                }
            }
            l = g_list_sort (list, price_cmp);
            list = l;
            for ( l = list; l; l=l->next)
            {
                gtk_list_store_append (priv->model_filtered, &iter);
                add_count++;
                do_price_widget_add_rec_to_model (priv->model_filtered, &iter, l->data);
                indexes = g_list_append (indexes, l->data);
                gtk_list_store_set (priv->model_filtered, &iter,
                                        15, TRUE,
                                        -1);
            }
            /*if ( add_count )
                gtk_list_store_set (priv->model_filtered, &iter,
                                        14, TRUE,
                                        -1);*/
            g_list_free (list);
        }
        PQclear(res);
    }

    if ( !priv->not_lexed )
    {

        sql = g_strdup_printf ("SELECT lex FROM _price_handbook_goods_lex"
                               " WHERE code = %d ORDER BY nn", record->goods_code);
        res = PQexec(priv->conn, sql);
        if ( PQresultStatus(res) != PGRES_TUPLES_OK )
        {
            ERROR("Connect to '%s'. %s", priv->connection_str, PQerrorMessage(priv->conn));
            return;
        }
        nrow = PQntuples(res);
        lexs = g_new0(char*, nrow+1);
        for ( i = 0; i < nrow; i++ )
            lexs[i] = g_strdup (PQgetvalue (res, i, 0));
        PQclear (res);
        g_free (sql);
        g_return_if_fail (nrow > 0);

        sql = NULL;

        goods = g_new0(gint*, nrow);
        len = g_new0(gint, nrow);

        for ( i = 0,count=0; i < nrow; i++,count++ )
        {
            len[i] = do_price_widget_get_goods_by_lex (widget, lexs[i], (i) ? goods[i-1] : NULL, &goods[i]);
            if ( len[i] < 0 )
                return;
            if ( len[i] == 1 && i ) break;
        }

        for ( j = count - 1; j >=0; j-- )
        {
            add_count = 0;
            list = NULL;

            for ( i = 0; i < len[j]; i++ )
            {
                flag = TRUE;
                if ( j != count -1 )
                    for ( k = 0; k < len[j+1]; k++ )
                        if ( goods[j][i] == goods[j+1][k] )
                        {
                            flag = FALSE;
                            break;
                        }
                if ( flag )
                {
                    l = do_price_widget_find_records_by_goods (widget, goods[j][i]);
                    for ( ;l; l=l->next )
                    {
                        DoPriceRecord *rec;
                        DoSupplierRecord *supplier;
                        rec = l->data;
                        supplier = g_hash_table_lookup (priv->suppliers, &rec->supplier_code);
                        if ( supplier->enabled )
                            list = g_list_append (list, l->data);
                    }
                }
            }
            l = g_list_sort (list, price_cmp);
            //g_list_free (list);
            list = l;
            for ( l = list ;l; l=l->next )
            {
                flag = TRUE;
                for ( indx=indexes; indx; indx=indx->next )
                    if ( ((DoPriceRecord*)indx->data)->uniq == ((DoPriceRecord*)l->data)->uniq )
                    {
                        flag = FALSE;
                        break;
                    }
                if ( flag )
                {
                    add_count++;
                    gtk_list_store_append (priv->model_filtered, &iter);
                    do_price_widget_add_rec_to_model (priv->model_filtered, &iter, l->data);
                }
            }
            g_list_free (list);

            if ( add_count )
                gtk_list_store_set (priv->model_filtered, &iter,
                                            14, TRUE,
                                            -1);
        }
        g_free (len);
        for ( i = 0; i < count; i++ )
            g_free (goods[i]);
        g_free (goods);
    }
    g_list_free (indexes);

    GtkTreePath *path;
    path = gtk_tree_path_new_first ();
    gtk_tree_model_get_iter (GTK_TREE_MODEL (priv->model_filtered) ,&iter, path);

    gtk_tree_selection_select_iter (gtk_tree_view_get_selection (GTK_TREE_VIEW (priv->view_filtered)), &iter);
    gtk_tree_view_set_cursor (GTK_TREE_VIEW (priv->view_filtered), path, NULL, FALSE);
    gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW (priv->view_filtered), path, NULL,
                TRUE, 0., 0.);
    gtk_tree_path_free (path);

}
static GList*
do_price_widget_find_records_by_supplier_product (DoPriceWidget *widget, gint supplier_code, const gchar *product_code)
{
    DoSupplierRecord *rec;
    rec = g_hash_table_lookup (widget->priv->suppliers, &supplier_code);
    if ( !rec ) return NULL;

    return g_hash_table_lookup (rec->products, product_code);
}
static GList*
do_price_widget_find_records_by_goods (DoPriceWidget *widget, gint goods_code)
{
    return g_hash_table_lookup (widget->priv->goods, &goods_code);
}
static void
do_price_widget_view_row_activated_cb (GtkTreeView       *tree_view,
               GtkTreePath       *path,
               GtkTreeViewColumn *column,
               DoPriceWidget *widget)
{
	DoPriceWidgetPrivate *priv = widget->priv;
	DoPriceRecord *rec;

    rec = do_price_model_get_record ((DoPriceModel*)priv->model, path);

    if ( !rec ) return;
    MESSAGE("%s:%s\n%s:%d\n%s:%s\n%s:%s\n%s:%s\n%s:%f\n%s:%d\n%s:%d\n%s:%d\n%s:%s\n%s:%d\n%s:%d",
            fields[0].name, rec->product_code,
            fields[1].name, rec->manufactor_code,
            fields[2].name, rec->lifetime ? g_strdup_printf("%2.2d %d",rec->lifetime->month,rec->lifetime->year): "",
            fields[3].name, rec->product_name,
            fields[4].name, rec->manufactor_name,
            fields[5].name, rec->price,
            fields[6].name, rec->stock,
            fields[7].name, rec->min,
            fields[8].name, rec->parcel,
            fields[9].name, rec->supplier_name,
            fields[10].name,rec->supplier_code,
            fields[11].name,rec->order);

}
static gboolean
blink_cb (gpointer data)
{
    DoPriceWidget *widget = data;

    if ( widget->priv->blink_state == widget->priv->price_state )
        widget->priv->blink_state = DO_PRICE_STATE_NORMAL;
    else
        widget->priv->blink_state = widget->priv->price_state;
    g_object_ref(widget->priv->button_icons[widget->priv->blink_state]);
    gtk_button_set_image (GTK_BUTTON (widget->priv->button_supplier), widget->priv->button_icons[widget->priv->blink_state]);
    return TRUE;
}
static void
do_price_widget_blink_button_supplier (DoPriceWidget *widget, gboolean blink)
{

    if ( blink && !widget->priv->blink_timeout )
    {
        g_object_ref(widget->priv->button_icons[widget->priv->price_state]);
        gtk_button_set_image (GTK_BUTTON (widget->priv->button_supplier), widget->priv->button_icons[widget->priv->price_state]);
        widget->priv->blink_state = widget->priv->price_state;
        widget->priv->blink_timeout = g_timeout_add(BLINK_TIMEOUT, blink_cb, widget);
        return;
    }
    if ( !blink && widget->priv->blink_timeout )
    {
        g_source_remove (widget->priv->blink_timeout);
        widget->priv->blink_timeout = 0;
        widget->priv->blink_state = widget->priv->price_state;
        g_object_ref(widget->priv->button_icons[widget->priv->price_state]);
        gtk_button_set_image (GTK_BUTTON (widget->priv->button_supplier), widget->priv->button_icons[widget->priv->price_state]);
    }
}
static void
do_price_widget_make_header (DoPriceWidget *widget)
{
    GtkWidget *w;

    GtkWidget *i;
    widget->priv->button_menu = w = gtk_menu_button_new ();
    i = gtk_image_new_from_icon_name ("view-list-symbolic", GTK_ICON_SIZE_MENU);
    gtk_button_set_image (GTK_BUTTON (w), i);
    if GTK_IS_HEADER_BAR (widget->priv->header)
    {
        gtk_header_bar_pack_end (GTK_HEADER_BAR (widget->priv->header), w);
    }
    else
    if GTK_IS_BOX (widget->priv->header)
    {
        gtk_box_pack_end (GTK_BOX (widget->priv->header), w, FALSE, FALSE, 0);
    }


    widget->priv->button_supplier = w = gtk_menu_button_new ();
    widget->priv->button_icons[2] = gtk_image_new_from_icon_name ("software-update-urgent-symbolic", GTK_ICON_SIZE_MENU);
    widget->priv->button_icons[1] = gtk_image_new_from_icon_name ("software-update-available-symbolic", GTK_ICON_SIZE_MENU);
    widget->priv->button_icons[0] = gtk_image_new_from_icon_name ("system-users-symbolic", GTK_ICON_SIZE_MENU);
    g_object_ref (widget->priv->button_icons[DO_PRICE_STATE_NORMAL]);
    gtk_button_set_image (GTK_BUTTON (w), widget->priv->button_icons[DO_PRICE_STATE_NORMAL]);

    if GTK_IS_HEADER_BAR (widget->priv->header)
    {
        gtk_header_bar_pack_end (GTK_HEADER_BAR (widget->priv->header), w);
    }
    else
    if GTK_IS_BOX (widget->priv->header)
    {
        gtk_box_pack_end (GTK_BOX (widget->priv->header), w, FALSE, FALSE, 0);
    }

    widget->priv->suppliers_widget = gtk_popover_menu_new();
    gtk_menu_button_set_popover (GTK_MENU_BUTTON (w), widget->priv->suppliers_widget);

    widget->priv->suppliers_view = do_price_widget_make_suppliers_view (widget);

    widget->priv->button_search = w = gtk_menu_button_new ();
    i = gtk_image_new_from_icon_name ("edit-find-symbolic", GTK_ICON_SIZE_MENU);
    gtk_button_set_image (GTK_BUTTON (w), i);
    if GTK_IS_HEADER_BAR (widget->priv->header)
    {
        gtk_header_bar_pack_end (GTK_HEADER_BAR (widget->priv->header), w);
    }
    else
    if GTK_IS_BOX (widget->priv->header)
    {
        gtk_box_pack_end (GTK_BOX (widget->priv->header), w, FALSE, FALSE, 0);
    }

    //gtk_widget_set_size_request (widget->priv->suppliers_view, 600,100);

//    gtk_container_add( GTK_CONTAINER (widget->priv->suppliers_widget), widget->priv->suppliers_view);
  //  gtk_widget_show_all (widget->priv->suppliers_view);


}
static gint
supplier_name_cmp (gconstpointer  a, gconstpointer  b)
{
    return strcmp (((DoSupplierRecord*)a)->name, ((DoSupplierRecord*)b)->name);
}

static GtkWidget*
do_price_widget_make_suppliers_view (DoPriceWidget *widget)
{
    GtkWidget *box;
    GtkWidget *hbox;
    GtkWidget *view;
    GtkWidget *button;
    GtkWidget *scroll;
    GtkTreeViewColumn   *column;
    GtkCellRenderer     *renderer;

    DoPriceWidgetPrivate *priv = widget->priv;

    box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 6);
    gtk_container_set_border_width (GTK_CONTAINER (box), 10);

    hbox = gtk_button_box_new (GTK_ORIENTATION_HORIZONTAL);
    gtk_button_box_set_layout (GTK_BUTTON_BOX (hbox), GTK_BUTTONBOX_EXPAND);

    button = gtk_button_new();
    gtk_button_set_image (GTK_BUTTON (button), gtk_image_new_from_icon_name ("view-refresh-symbolic", GTK_ICON_SIZE_BUTTON));
    gtk_container_add( GTK_CONTAINER (hbox), button);

    button = gtk_button_new();
    gtk_button_set_image (GTK_BUTTON (button), gtk_image_new_from_icon_name ("object-select-symbolic", GTK_ICON_SIZE_BUTTON));
    gtk_container_add( GTK_CONTAINER (hbox), button);

	gtk_box_pack_start (GTK_BOX (box), hbox, FALSE, FALSE, 0);



    view = do_tree_view_new ("PriceSuppliers");
    scroll = gtk_scrolled_window_new (NULL, NULL);

    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scroll), GTK_SHADOW_IN);
    gtk_container_add (GTK_CONTAINER (scroll), view);

    gtk_box_pack_start (GTK_BOX (box), scroll, TRUE, TRUE, 0);

    gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (view)), GTK_SELECTION_NONE);
    gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (view), FALSE);

    column = do_tree_view_column_new(DO_TREE_VIEW (view), "name", "", "АПТЕКА ХОЛДИНГ");

    gtk_tree_view_column_set_spacing (column, 6);

    renderer = gtk_cell_renderer_toggle_new ();
    gtk_tree_view_column_pack_start (column, renderer, FALSE);
    g_signal_connect (renderer, "toggled", G_CALLBACK (do_price_widget_supplier_toggled_cb), widget);
    gtk_tree_view_column_add_attribute (column, renderer, "active", 5);

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_column_pack_start (column, renderer, TRUE);
    gtk_tree_view_column_set_reorderable(column, TRUE);
    gtk_tree_view_column_add_attribute (column, renderer, "text", 1);
    gtk_tree_view_column_add_attribute (column, renderer, "foreground-set", 4);
    g_object_set (renderer, "foreground", "red", NULL);

//    gtk_tree_view_column_add_attribute (column, renderer, "background-set", 4);
    //g_object_set (renderer, "background-rgba", &alert_color, NULL);
    do_tree_view_append_column (DO_TREE_VIEW (view), column);

    column = do_tree_view_column_new(DO_TREE_VIEW (view), "time", "", "06.07.2015 15:55");
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_column_pack_start (column, renderer, TRUE);
    gtk_tree_view_column_set_reorderable(column, TRUE);
    gtk_tree_view_column_add_attribute (column, renderer, "markup", 3);
    //gtk_tree_view_column_add_attribute (column, renderer, "background-set", 4);
    //g_object_set (renderer, "background-rgba", &alert_color, NULL);
    gtk_tree_view_column_add_attribute (column, renderer, "foreground-set", 4);
    g_object_set (renderer, "foreground", "red", NULL);

    do_tree_view_append_column (DO_TREE_VIEW (view), column);

    /*!!column = do_tree_view_column_new(DO_TREE_VIEW (view), "check", "", "ПР");
    renderer = gtk_cell_renderer_toggle_new ();
    gtk_tree_view_column_pack_start (column, renderer, TRUE);
    gtk_tree_view_column_set_reorderable(column, TRUE);
    gtk_tree_view_column_add_attribute (column, renderer, "active", 5);
    do_tree_view_append_column (DO_TREE_VIEW (view), column);

    g_signal_connect (renderer, "toggled", G_CALLBACK (do_price_widget_supplier_toggled_cb), widget);
    */


    priv->suppliers_model = gtk_list_store_new (6, G_TYPE_INT, G_TYPE_STRING, G_TYPE_DOUBLE, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN);
    gtk_tree_view_set_fixed_height_mode (GTK_TREE_VIEW (view), TRUE);

    gtk_tree_view_set_model (GTK_TREE_VIEW (view), GTK_TREE_MODEL (priv->suppliers_model));
    return box;
}
static gchar*
_g_date_time_format_human (GDateTime *time)
{
    gchar *val;
    GDateTime *today = g_date_time_new_now_local(), *yesterday;
    today = g_date_time_new_now_local ();
    gint y1,y2,m1,m2,d1,d2,y3,d3,m3;
    if ( time )
    {
        g_date_time_get_ymd(today, &y1, &m1, &d1);
        g_date_time_get_ymd(time, &y2, &m2, &d2);
        if ( y1 == y2 && m1 == m2 && d1 == d2 )
            val = g_date_time_format (time, "%H:%M");
        else {
            yesterday = g_date_time_add_days(today, -1);
            g_date_time_get_ymd(yesterday, &y3, &m3, &d3);
            if ( y3 == y2 && m3 == m2 && d3 == d2 )
                val = g_date_time_format (time, "вчера %H:%M");
            else {
                if ( y1 == y2 )
                    val = g_date_time_format (time, "%d %b, %H:%M");
                else
                    val = g_date_time_format (time, "%d.%m.%y %H:%M");
            }
            g_date_time_unref (yesterday);
        }
    }
    else
        val = g_strdup("");
    g_date_time_unref (today);
    return val;
}

static gboolean
do_price_widget_update_suppliers_model_iter (GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer data)
{
    DoPriceWidgetPrivate *priv = DO_PRICE_WIDGET (data)->priv;
    GValue code={0,};
    gint supplier_code;
    DoSupplierRecord *rec;

    gtk_tree_model_get_value (model, iter, 0, &code);
    supplier_code = g_value_get_int (&code);

    rec = g_hash_table_lookup (priv->suppliers, &supplier_code);

    g_assert (rec != NULL);

    gchar *val;

    val = _g_date_time_format_human (rec->time);
    if ( rec->crnttime )
    {
        if ( !rec->time || !g_date_time_equal( rec->time, rec->crnttime) )
        {
            gchar *buf,*val1;
            val1 = _g_date_time_format_human (rec->crnttime);
            buf = g_strdup_printf("%s/<i>%s</i>", val,val1);
            g_free (val);g_free (val1);
            val = buf;
        }
    }
    gtk_list_store_set (GTK_LIST_STORE (model), iter, 3, val, 4, do_price_widget_not_in_day (rec->time), -1);

    return FALSE;

}

static void
do_price_widget_fill_suppliers_model (DoPriceWidget *widget)
{
    DoPriceWidgetPrivate *priv = widget->priv;

    GList *l, *list = NULL;

    list = g_hash_table_get_values(priv->suppliers);

    l = g_list_sort (list, supplier_name_cmp);
    //g_list_free (list);
    list = l;

    for ( l = list; l; l=l->next )
    {
        GtkTreeIter iter;
        DoSupplierRecord *rec;
        rec = l->data;
        gtk_list_store_append (priv->suppliers_model, &iter);
        gtk_list_store_set (priv->suppliers_model, &iter,
                             0, rec->code,
                             1, rec->name,
                             2, 0.,
                             5, rec->enabled,
                             -1);
    }
    g_list_free (list);

    gtk_tree_model_foreach (GTK_TREE_MODEL (priv->suppliers_model), do_price_widget_update_suppliers_model_iter, widget);

    //gtk_widget_set_size_request (widget->priv->suppliers_view, -1, 300);
    gtk_widget_show_all (widget->priv->suppliers_view);
    gtk_container_add( GTK_CONTAINER (widget->priv->suppliers_widget), widget->priv->suppliers_view);
}
static void
do_price_widget_update_filter (DoPriceWidget *widget)
{
    GtkTreePath *path;
    do_price_widget_check_outdated (widget);

    widget->priv->selected_record = NULL;
    path = gtk_tree_path_new_first ();

    gtk_tree_view_set_cursor (GTK_TREE_VIEW (widget->priv->view), path, NULL, FALSE);

    gtk_tree_path_free (path);

}

static void
do_price_widget_supplier_toggled_cb (GtkCellRendererToggle *cell_renderer,
                                                  gchar                 *path,
                                                  DoPriceWidget          *widget)
{
    DoPriceWidgetPrivate *priv = widget->priv;
    DoSupplierRecord *record;
    GtkTreePath *p;
    GtkTreeIter iter;
    GValue value = {0,};
    gint code;
    GValue supplier = {0,};
    p = gtk_tree_path_new_from_string (path);
    gtk_tree_model_get_iter (GTK_TREE_MODEL (priv->suppliers_model), &iter, p);
    gtk_tree_path_free (p);

    gtk_tree_model_get_value (GTK_TREE_MODEL (priv->suppliers_model), &iter, 5, &value);
    gtk_tree_model_get_value (GTK_TREE_MODEL (priv->suppliers_model), &iter, 0, &supplier);
    code = g_value_get_int (&supplier);

    record = g_hash_table_lookup (priv->suppliers, &code);
    record->enabled = !g_value_get_boolean (&value);

    gtk_list_store_set (priv->suppliers_model, &iter, 5, record->enabled, -1);


    if ( do_price_model_filter_set_supplier_visible (priv->model, record->code, record->enabled) )
        do_price_widget_update_filter (widget);
    do_price_widget_check_outdated (widget);

}
static gboolean
do_price_model_filter_set_supplier_visible (DoPriceModel *model, gint supplier_code, gboolean visible)
{

    gint n_record;
    DoPriceRecord **records, *record_all,*record_filtered;
    int indx_all, indx_filtered;
    GtkTreePath *path;
    GtkTreeIter iter;
    gboolean modify = FALSE;

    records = g_new0 (DoPriceRecord*, model->n_record_all);
    n_record = 0;

    for ( indx_all = 0,indx_filtered=0;  indx_all < model->n_record_all; indx_all++)
    {
        record_all = model->records_all[indx_all];
        if ( indx_filtered < model->n_record_filtered )
            record_filtered = model->records_filtered[indx_filtered];
        else
            record_filtered = NULL;

        if ( record_all->supplier_code == supplier_code )
        {
            if ( !visible )
            {
                if ( record_filtered && record_filtered->uniq == record_all->uniq )
                {
                    path = gtk_tree_path_new();
                    gtk_tree_path_append_index(path, n_record);
                    gtk_tree_model_row_deleted (GTK_TREE_MODEL (model), path);
                    gtk_tree_path_free (path);
                    indx_filtered++;
                    modify = TRUE;
                }
            }
            else
            {
                if ( !record_filtered || record_filtered->uniq != record_all->uniq )
                {
                    records[n_record] = record_all;
                    records[n_record]->indx1 = n_record;
                    n_record++;

                    path = gtk_tree_path_new();
                    gtk_tree_path_append_index(path, n_record-1);
                    iter.stamp      = model->stamp;
                    iter.user_data  = record_all;
                    iter.user_data2 = NULL;
                    iter.user_data3 = NULL;

                    gtk_tree_model_row_inserted (GTK_TREE_MODEL (model), path, &iter);
                    gtk_tree_path_free (path);
                    modify = TRUE;
                }
                else
                {
                    records[n_record] = record_all;
                    records[n_record]->indx1 = n_record;
                    n_record++;
                    indx_filtered++;
                }
            }
        }
        else
        {
            if ( record_filtered && record_filtered->uniq == record_all->uniq )
            {
                records[n_record] = record_all;
                records[n_record]->indx1 = n_record;
                n_record++;
                indx_filtered++;
            }
        }
    }
    if ( modify )
    {
        g_free (model->records_filtered);
        model->n_record_filtered = n_record;
        model->records_filtered = records;

    }
    return modify;
}
static GtkTreePath*
do_price_model_search_func(DoPriceModel *model,
						gint column,
						const gchar *key,
						gpointer user_data)
{
    gchar *str,*key_;
    gint len;
    GtkTreePath *path;

    str = g_utf8_normalize (key, -1, G_NORMALIZE_ALL);
    key_ = g_utf8_casefold (str, -1);
    len = strlen(key_);
    path = 0;


    int l = 0, h = model->n_record_filtered - 1, i, c;
    int result = 0;
    while ( l <= h )
    {
        i = (l + h) >> 1;
        if ( i > model->n_record_filtered- 1 )
           i = model->n_record_filtered;
        c = strncmp(model->records_filtered[i]->search_name, key_, len);
        if ( c < 0 ) l = i + 1;
        else
        {
            h = i - 1;
            if ( !c )
            {
                while ( i > 0 && !strncmp(model->records_filtered[i]->search_name, key_, len) ) i--;
                l = i;
                result = 1;
            }
        }
    }
    if ( result )
        path = gtk_tree_path_new_from_indices(l,-1);
    g_free(str);
    g_free(key_);
    return path;
}
static gboolean
do_price_widget_check_update_prices (DoPriceWidget *widget)
{
    DoPriceWidgetPrivate *priv = widget->priv;
    g_hash_table_foreach (priv->suppliers, (GHFunc)do_price_widget_supplier_get_last_prices, widget);
    do_price_widget_check_outdated (widget);
    gtk_tree_model_foreach (GTK_TREE_MODEL (priv->suppliers_model), do_price_widget_update_suppliers_model_iter, widget);
    return TRUE;
}
