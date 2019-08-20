#ifndef MISC_H_INCLUDED
#define MISC_H_INCLUDED

#include <glib.h>
#include <gtk/gtk.h>
#include "do_progress_info.h"
#include "exchange.h"
#include "do_link_dialog.h"
#include <domino.h>

#define PROGRESS_DETAIL(...) { if (get_progress()) { gchar *v = g_strdup_printf(__VA_ARGS__); do_progress_info_set_details(get_progress(), v); g_free(v);}}
#define PROGRESS_STATUS(...) { if (get_progress()) { gchar *v = g_strdup_printf(__VA_ARGS__); do_progress_info_set_status(get_progress(), v); g_free(v);}}
#define PROGRESS_PROGRESS(a,b) { if (get_progress()) do_progress_info_set_progress(get_progress(), a, b); }
#define PROGRESS_PULSE() { if (get_progress()) do_progress_info_pulse_progress(get_progress()); }
#define CANCELLED ( get_progress() && g_cancellable_is_cancelled(get_cancellable()) )
#define DEBUG(...) { if (priv->debug) {gchar *v = g_strdup_printf(__VA_ARGS__); priv->debug_errors=g_slist_append(priv->debug_errors, v); } }

typedef struct _MiscGtkTreeModelFindColumn MiscGtkTreeModelFindColumn;

struct _MiscGtkTreeModelFindColumn {
    const gchar *text;
    gint column;
    gint column_cache;
};


#ifdef __cplusplus
extern "C" {
#endif

gboolean str_iqual(const gchar *s1, const gchar *s2);
gboolean str_contain(const gchar *s1, const gchar *);

DoProgressInfo *get_progress();
void set_progress(DoProgressInfo *progress);
GCancellable  *get_cancellable();
void set_cancellable(GCancellable *c);

gboolean config_file_init();
gboolean config_file_save();
void window_configure_save(GtkWidget *widget, gchar *path);
void window_configure_init(GtkWidget *widget, gchar *path, gint width, gint height);

gint text_length(const gchar *text);
gboolean strpdatetime(const gchar *str, struct tm *tm);
//gchar *strfdatetime(struct tm *tm);

GSList *text_to_lexs(const gchar *text);
gboolean lexs_compare(GSList *l1, GSList *l2, gint min_match);

GtkTreePath *misc_find_by_name_from_model(GtkTreeModel *model, GSList *MiscGtkTreeModelFindColumns);

gboolean misc_link_handbook(do_alias_t *alias, GSList *objs_1c, DoExchangeHandbook handbook, void *handbook_key, gboolean empty_enable);
void wait_for(gboolean *event);


#ifdef __cplusplus
}
#endif


#endif // MISC_H_INCLUDED
