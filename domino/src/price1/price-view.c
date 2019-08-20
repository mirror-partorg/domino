#include "price-view.h"

#include "../misc/gtk_tree_cell_data_func.h"
#include <libpq-fe.h>


typedef struct _PriceViewPrivate PriceViewPrivate;
static void price_view_destroy_cb(GtkTreeView *view, PriceViewPrivate *priv);
static gboolean price_view_key_press_event_cb(GtkWidget *widget, GdkEventKey *event, PriceViewPrivate *priv);
static GtkTreeModel *price_model_new(const gchar *conn_str, const gchar *firm_nick);

#define DEFAULT_COLUMNS "order,product_name,price,stock,lifetime,firm_name,manufactor_name"
static gchar *settings;

struct _PriceViewPrivate
{
    gchar        search_text[1024];
    gint         search_char_count;
    gint         search_item;
    guint        search_flush_timeout;
};

