
#ifndef DO_SEARCH_H
#define DO_SEARCH_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define DO_TYPE_SEARCH         (do_search_get_type ())
#define DO_SEARCH(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_SEARCH, DoSearch))
#define DO_SEARCH_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_SEARCH, DoSearchClass))
#define DO_IS_SEARCH(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_SEARCH))
#define DO_IS_SEARCH_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_SEARCH))
#define DO_SEARCH_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_SEARCH, DoSearchClass))

typedef struct _DoSearch		DoSearch;
typedef struct _DoSearchPrivate	DoSearchPrivate;
typedef struct _DoSearchClass		DoSearchClass;

typedef enum
{
	DO_SEARCH_FLAGS_SHOW_LABEL	= 1 << 0,
	DO_SEARCH_FLAGS_SHOW_BUTTONS = 1 << 1,

} DoSearchFlags;

struct _DoSearch
{
	GtkHBox parent_instance;
};

struct _DoSearchClass
{
	GtkHBoxClass parent_class;

	void	 (* find)  (DoSearch *search, gboolean direction, gint creterion, const gchar *text, gpointer data);
};

GType		       do_search_get_type(void);

GtkWidget         *do_search_new(GtkWidget *parent, DoSearchFlags flags);
void               do_search_show(DoSearch *search, gint criterion);
gint               do_search_add_criterion(DoSearch *search, const gchar *title);

G_END_DECLS

#endif /* !DO_SEARCH_H */
