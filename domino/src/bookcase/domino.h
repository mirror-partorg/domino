#ifndef DOMINOX_H_INCLUDED
#define DOMINOX_H_INCLUDED

#include <gtk/gtk.h>

#include "do_context.h"
#include "do_utilx.h"
#include "do_view.h"
#include "do_message.h"


G_BEGIN_DECLS

typedef enum {
    DOMINO_CONFIG_COMMON,
    DOMINO_CONFIG_PROFILE,
    DOMINO_CONFIG_LOCAL,
    DOMINO_CONFIG_N
} DominoConfigType;

typedef struct {
    gint   code;
    gchar *name;
} DoProperyItem;

#define DOMINO_INIT dominox_init()
#define DOMINO_FINALIZE() {domino_finalize(); dominox_finalize();}
#define DOMINO_PROFILE_SET(a, b, ...) \
       {domino_config_property_set(DOMINO_CONFIG_PROFILE, a, __VA_ARGS__); \
       domino_config_kind_set(DOMINO_CONFIG_PROFILE, a, b); \
       domino_config_save(DOMINO_CONFIG_PROFILE, FALSE);}
#define DOMINO_PROFILE_OBJECT_INIT(a, b, ...) \
       domino_config_property_to_object(DOMINO_CONFIG_PROFILE, a, b, __VA_ARGS__)

#define DOMINO_PROFILE_GET(a, ...) \
       domino_config_property_get(DOMINO_CONFIG_PROFILE, a, __VA_ARGS__)

#define DOMINO_PROFILE_PROPERTIES(a) \
       domino_config_properties(DOMINO_CONFIG_PROFILE, a)

#define DOMINO_COMMON_SET(a, b, ...) \
       {domino_config_property_set(DOMINO_CONFIG_COMMON, a, __VA_ARGS__); \
       domino_config_kind_set(DOMINO_CONFIG_COMMON, a, b); \
       domino_config_save(DOMINO_CONFIG_COMMON, FALSE);}
#define DOMINO_COMMON_OBJECT_INIT(a, b, ...) \
       domino_config_property_to_object(DOMINO_CONFIG_COMMON, a, b, __VA_ARGS__)

#define DOMINO_COMMON_GET(a, ...) \
       domino_config_property_get(DOMINO_CONFIG_COMMON, a, __VA_ARGS__)

#define DOMINO_COMMON_GET_ATTR(a, ...) \
       domino_config_property_get_attr(DOMINO_CONFIG_COMMON, a, __VA_ARGS__)

#define DOMINO_COMMON_PROPERTIES(a) \
       domino_config_properties(DOMINO_CONFIG_COMMON, a)


#define DOMINO_LOCAL_SET_KIND(a, b, ...) \
       {domino_config_property_set(DOMINO_CONFIG_LOCAL, a, __VA_ARGS__); \
       domino_config_kind_set(DOMINO_CONFIG_LOCAL, a, b); \
       domino_config_save(DOMINO_CONFIG_LOCAL, FALSE);}
#define DOMINO_LOCAL_OBJECT_INIT(a, b, ...) \
       domino_config_property_to_object(DOMINO_CONFIG_LOCAL, a, b, __VA_ARGS__)

#define DOMINO_LOCAL_GET(a, ...) \
       domino_config_property_get(DOMINO_CONFIG_LOCAL, a, __VA_ARGS__)
#define DOMINO_LOCAL_SET(a, ...) \
       domino_config_property_set(DOMINO_CONFIG_LOCAL, a, __VA_ARGS__)

#define DOMINO_LOCAL_PROPERTIES(a) \
       domino_config_properties(DOMINO_CONFIG_LOCAL, a)

#define DOMINO_SETTING_TIMEOUT 5000
#define TEMPLATE_REPORT_DEFAULT "report-default"

#define gtk_dialog_run domino_dialog_run

int dominox_init();
void dominox_finalize();
gboolean domino_config_save(gint type, gboolean now);
gboolean domino_config_save_text(gint type, const char *text);
int domino_config_read(gint type);
void domino_config_property_to_object(gint type, GObject *widget, const gchar *path, ...) DO_GNUC_NULL_TERMINATED;
void domino_config_property_from_object(gint type, GObject *widget, const gchar *path, ...) DO_GNUC_NULL_TERMINATED;
void domino_config_property_set(gint type, const gchar *path, ...) DO_GNUC_NULL_TERMINATED;
GList *domino_config_properties(gint type, const gchar *path);
void domino_config_property_get(gint type, const gchar *path, ...) DO_GNUC_NULL_TERMINATED;
void domino_config_property_get_attr(gint type, const gchar *path, ...) DO_GNUC_NULL_TERMINATED;
void domino_config_kind_set(gint type, const gchar *path, const gchar *kind);
GFileInputStream *domino_get_config_stream(gint type);
gchar *domino_config_filename(gint type);
gboolean domino_check_access(int roles);
gboolean restart_programm(gchar *message);
int domino_back_window_focus();
int domino_dialog_run(GtkDialog *dialog);
GtkWindow *domino_get_top_window(int *dialog);


void update_action_set_arg(char **argv, gint argc);
#if defined(BINARY_UPDATE)
void update_action_run(GtkWidget *main_win, gboolean in_thread);
#endif

gchar *util_create_uniq_filename(const gchar *path, const gchar *basename, const gchar *fileext);
gboolean util_save_to_file(const gchar *uri, gchar *text, gint len);
GFileInputStream *domino_get_template_stream(const gchar* name);

gboolean domino_get_ui(gchar **ui, gsize *length);

void domino_log_gtk_func(const gchar *log_domain, GLogLevelFlags log_flags, const gchar *msg, gpointer user_data);

void do_property_list_free(do_sort_list_t *list);
gchar *do_property_list_get_name(do_sort_list_t *list, int code);
do_sort_list_t *do_property_list_create(const gchar *section);
do_sort_list_t *do_property_list_create_with_store(const gchar *section, GtkListStore *stores);
GtkListStore *do_property_enum_create_store(const gchar *enum_name, gchar **default_value);

void domino_update_main_window_title();
GtkWindow *domino_get_main_window();
GdkWindow *domino_get_back_window();
void domino_set_main_window(GtkWindow *window);
gchar *domino_get_default_location_name();
GtkBuilder *do_builder_new(const gchar *name);
GtkWidget *do_builder_get_widget(GtkBuilder *builder, const gchar *name);
#if GTK_MAJOR_VERSION > 2
GAction *do_builder_get_action(GtkBuilder *builder, const gchar *name);
#else
GtkAction *do_builder_get_action(GtkBuilder *builder, const gchar *name);
#endif
#ifdef CASH
int domino_send_barcode_eventkey_to_crntwin(char *barcode);
#endif


G_END_DECLS

#endif // DOMINOX_H_INCLUDED
