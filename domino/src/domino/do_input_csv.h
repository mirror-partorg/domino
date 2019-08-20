
#ifndef DO_INPUT_CSV_H
#define DO_INPUT_CSV_H

#include <gio/gio.h>

G_BEGIN_DECLS

#define DO_INPUT_CSV_TYPE        (do_input_csv_get_type ())
#define DO_INPUT_CSV(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_INPUT_CSV_TYPE, DoInputCsv))
#define DO_IS_INPUT_CSV(o)       (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_INPUT_CSV_TYPE))

typedef struct _DoInputCsv DoInputCsv;

GType do_input_csv_get_type      (void) G_GNUC_CONST;

DoInputCsv    *do_input_csv_new_from_uri(const char *uri, GError **error);
gboolean       do_input_csv_read_line  	(DoInputCsv *csv, GError **error);
gchar 	      *do_input_csv_get_string	(DoInputCsv *csv);
gint           do_input_csv_get_int 	(DoInputCsv *csv);
gdouble	       do_input_csv_get_double	(DoInputCsv *csv, gchar **end_value);
gboolean       do_input_csv_is_name     (DoInputCsv *csv, const gchar *name);
gint           do_input_csv_crnt_line   (DoInputCsv *csv);

G_END_DECLS

#endif /* DO_INPUT_CSV_H */
