
#ifndef __DO_ENUM_VALUE_H__
#define __DO_ENUM_VALUE_H__

#include <gtk/gtk.h>
#include <domino.h>

G_BEGIN_DECLS

GType do_enum_value_kind_get_type (void) G_GNUC_CONST;
#define DO_TYPE_ENUM_VALUE_KIND (do_enum_value_kind_get_type())

typedef struct _DoEnumValue   DoEnumValue;
typedef struct _DoEnumValueClass DoEnumValueClass;

#define DO_TYPE_ENUM_VALUE              (do_enum_value_get_type ())
#define DO_ENUM_VALUE(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), DO_TYPE_ENUM_VALUE, DoEnumValue))
#define DO_ENUM_VALUE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), DO_TYPE_ENUM_VALUE, DoEnumValueClass))
#define DO_IS_ENUM_VALUE(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), DO_TYPE_ENUM_VALUE))
#define DO_IS_ENUM_VALUE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), DO_TYPE_ENUM_VALUE))
#define DO_ENUM_VALUE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), DO_TYPE_ENUM_VALUE, DoEnumValueClass))

struct _DoEnumValue {
    GObject parent_instance;
};

struct _DoEnumValueClass {
    GObjectClass parent_class;
};

GType    do_enum_value_get_type   (void) G_GNUC_CONST;
DoEnumValue  *do_enum_value_new();

gint         do_enum_value_get_int(DoEnumValue *enum_value);
gdouble      do_enum_value_get_double(DoEnumValue *enum_value);
const gchar *do_enum_value_get_string(DoEnumValue *enum_value);
gboolean     do_enum_value_get_boolean(DoEnumValue *enum_value);

void         do_enum_value_set_int(DoEnumValue *enum_value, gint value);
void         do_enum_value_set_double(DoEnumValue *enum_value, gdouble value);
void         do_enum_value_set_string(DoEnumValue *enum_value, const gchar *value);
void         do_enum_value_set_boolean(DoEnumValue *enum_value, gboolean value);

G_END_DECLS

#endif /* __DO_ENUM_VALUE_H__ */
