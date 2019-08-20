#ifndef STORAGE_DB_H_INCLUDED
#define STORAGE_DB_H_INCLUDED

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS


typedef struct _StorageDb   StorageDb;
typedef struct _StorageDbClass StorageDbClass;

#define STORAGE_TYPE_DB              (storage_db_get_type ())
#define STORAGE_DB(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), STORAGE_TYPE_DB, StorageDb))
#define STORAGE_DB_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), STORAGE_TYPE_DB, StorageDbClass))
#define STORAGE_IS_DB(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), STORAGE_TYPE_DB))
#define STORAGE_IS_DB_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), STORAGE_TYPE_DB))
#define STORAGE_DB_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), STORAGE_TYPE_DB, StorageDbClass))


struct _StorageDb {
    GObject parent_instance;
};

struct _StorageDbClass {
    GObjectClass parent_class;
};

GType           storage_db_get_type (void) G_GNUC_CONST;
StorageDb      *storage_db_new(const gchar *connection_info);
gboolean        storage_db_connect(StorageDb *storage);
gchar          *storage_db_get_error_replace(StorageDb *storage, const gchar *name);

G_END_DECLS



#endif // STORAGE-DB_H_INCLUDED
