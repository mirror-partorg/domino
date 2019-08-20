
#ifndef __DO_CONTEXT_H__
#define __DO_CONTEXT_H__

#include <gtk/gtk.h>
#include <domino.h>

G_BEGIN_DECLS

typedef enum
{
    DO_CONTEXT_KIND_UNDEFINE,
    DO_CONTEXT_KIND_PRODUCT,
    DO_CONTEXT_KIND_DOCUMENT,
    DO_CONTEXT_KIND_LINE,
    DO_CONTEXT_KIND_REGION,
    DO_CONTEXT_KIND_DTYPE,
    DO_CONTEXT_KIND_CHECK,
    DO_CONTEXT_KIND_PARTNER,
    DO_CONTEXT_KIND_SKLAD,
} DoContextKind;

GType do_context_kind_get_type (void) G_GNUC_CONST;
#define DO_TYPE_CONTEXT_KIND (do_context_kind_get_type())

typedef struct _DoContext   DoContext;
typedef struct _DoContextClass DoContextClass;

#define DO_TYPE_CONTEXT              (do_context_get_type ())
#define DO_CONTEXT(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), DO_TYPE_CONTEXT, DoContext))
#define DO_CONTEXT_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), DO_TYPE_CONTEXT, DoContextClass))
#define DO_IS_CONTEXT(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), DO_TYPE_CONTEXT))
#define DO_IS_CONTEXT_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), DO_TYPE_CONTEXT))
#define DO_CONTEXT_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), DO_TYPE_CONTEXT, DoContextClass))

struct _DoContext {
    GObject parent_instance;


};

struct _DoContextClass {
    GObjectClass parent_class;

};


GType       do_context_get_type   (void) G_GNUC_CONST;
DoContext  *do_context_new();
DoContext  *do_context_new_with_attributes(DoContextKind kind, const gchar *firt_property_name, ...);
DoContext  *do_context_copy (DoContext *context);

DoContextKind do_context_get_kind(DoContext *context);
void        do_context_set_kind(DoContext *context, DoContextKind kind);
gint        do_context_compare(DoContext *a, DoContext *b);

void        do_context_set_document_order_key0(DoContext *context, do_alias_t *alias, document_order_key0_t *document_order_key0);
void        do_context_get_document_order_key0(DoContext *context, do_alias_t *alias, document_order_key0_t *document_order_key0);


G_END_DECLS

#endif /* __DO_CONTEXT_H__ */
