
#ifndef __DO_REPORT_PLUGIN_H__
#define __DO_REPORT_PLUGIN_H__

#include <gtk/gtk.h>
#include <libxml/tree.h>
#include "do_progress_info.h"

G_BEGIN_DECLS

typedef struct _DoReportPlugin   DoReportPlugin;
typedef struct _DoReportPluginClass DoReportPluginClass;

#define DO_TYPE_REPORT_PLUGIN              (do_report_plugin_get_type ())
#define DO_REPORT_PLUGIN(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), DO_TYPE_REPORT_PLUGIN, DoReportPlugin))
#define DO_REPORT_PLUGIN_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), DO_TYPE_REPORT_PLUGIN, DoReportPluginClass))
#define DO_IS_REPORT_PLUGIN(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), DO_TYPE_REPORT_PLUGIN))
#define DO_IS_REPORT_PLUGIN_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), DO_TYPE_REPORT_PLUGIN))
#define DO_REPORT_PLUGIN_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), DO_TYPE_REPORT_PLUGIN, DoReportPluginClass))

#define SHEETS_ALIAS "ЛИСТЫ"

#define PROPERTY_TYPE "тип"
#define PROPERTY_KIND "вид"
#define PROPERTY_HEIGHT "высота"
#define PROPERTY_WIDTH "ширина"

#define PROPERTY_VALUE_BLOCK "блок"
#define PROPERTY_VALUE_VERTICAL "вертикальный"
#define PROPERTY_VALUE_HORIZONTAL "вертикальный"
#define PROPERTY_VALUE_AUTO "авто"


struct _DoReportPlugin {
    GObject parent_instance;
};

struct _DoReportPluginClass {
    GObjectClass parent_class;
};


GType           do_report_plugin_get_type   (void) G_GNUC_CONST;
DoReportPlugin *do_report_plugin_new(const gchar *app_name);
void            do_report_plugin_set_progress_info(DoReportPlugin *plugin, DoProgressInfo *progress);
void            do_report_plugin_set_template_filename(DoReportPlugin *plugin, const gchar *filename);
gchar          *do_report_plugin_get_template_filename(DoReportPlugin *plugin);
void            do_report_plugin_set_template_uri(DoReportPlugin *plugin, const gchar *uri);
gchar          *do_report_plugin_get_template_uri(DoReportPlugin *plugin);
void            do_report_plugin_set_values(DoReportPlugin *plugin, xmlDoc *values);
xmlDoc         *do_report_plugin_get_values(DoReportPlugin *plugin);
gboolean        do_report_plugin_is_done(DoReportPlugin *plugin);
gboolean        do_report_plugin_get_debug(DoReportPlugin *plugin);
void            do_report_plugin_set_debug(DoReportPlugin *plugin, gboolean debug);
gboolean        do_report_plugin_get_readonly(DoReportPlugin *plugin);
void            do_report_plugin_set_readonly(DoReportPlugin *plugin, gboolean readonly);
gchar          *do_report_plugin_get_title(DoReportPlugin *plugin);
void            do_report_plugin_set_title(DoReportPlugin *plugin, const gchar *title);
GtkWindow      *do_report_plugin_get_toplevel(DoReportPlugin *plugin);
GtkWindow      *do_report_plugin_show(DoReportPlugin *plugin);
GThread        *do_report_plugin_make_in_thread(DoReportPlugin *plugin, GSourceFunc exit_func, gpointer data);


G_END_DECLS

#endif /* __DO_REPORT_PLUGIN_H__ */
