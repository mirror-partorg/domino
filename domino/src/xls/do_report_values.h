
#ifndef __DO_REPORT_PLUGIN_H__
#define __DO_REPORT_PLUGIN_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS


typedef struct _DoReportPlugin   DoReportPlugin;
typedef struct _DoReportPluginClass DoReportPluginClass;

#define DO_TYPE_REPORT_PLUGIN              (do_report_plugin_get_type ())
#define DO_REPORT_PLUGIN(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), DO_TYPE_REPORT_PLUGIN, DoReportPlugin))
#define DO_REPORT_PLUGIN_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), DO_TYPE_REPORT_PLUGIN, DoReportPluginClass))
#define DO_IS_REPORT_PLUGIN(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), DO_TYPE_REPORT_PLUGIN))
#define DO_IS_REPORT_PLUGIN_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), DO_TYPE_REPORT_PLUGIN))
#define DO_REPORT_PLUGIN_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), DO_TYPE_REPORT_PLUGIN, DoReportPluginClass))

struct _DoReportPlugin {
    GObject parent_instance;
};

struct _DoReportPluginClass {
    GObjectClass parent_class;
};


GType       do_report_plugin_get_type   (void) G_GNUC_CONST;
DoReportPlugin  *do_report_plugin_new();

G_END_DECLS

#endif /* __DO_REPORT_PLUGIN_H__ */
