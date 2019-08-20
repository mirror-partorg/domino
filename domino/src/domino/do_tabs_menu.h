
#ifndef DO_TABS_MENU_H
#define DO_TABS_MENU_H

#include "do_window.h"

G_BEGIN_DECLS

#define DO_TYPE_TABS_MENU		(do_tabs_menu_get_type ())
#define DO_TABS_MENU(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), DO_TYPE_TABS_MENU, DoTabsMenu))
#define DO_TABS_MENU_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), DO_TYPE_TABS_MENU, DoTabsMenuClass))
#define DO_IS_TABS_MENU(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), DO_TYPE_TABS_MENU))
#define DO_IS_TABS_MENU_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), DO_TYPE_TABS_MENU))
#define DO_TABS_MENU_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), DO_TYPE_TABS_MENU, DoTabsMenuClass))

typedef struct _DoTabsMenu DoTabsMenu;
typedef struct _DoTabsMenuClass DoTabsMenuClass;
typedef struct _DoTabsMenuPrivate DoTabsMenuPrivate;

struct _DoTabsMenuClass
{
	GObjectClass parent_class;
};

struct _DoTabsMenu
{
	GObject parent_object;

	/*< private >*/
	DoTabsMenuPrivate *priv;
};

GType		do_tabs_menu_get_type		(void);

DoTabsMenu   *do_tabs_menu_new		(DoWindow *window);

G_END_DECLS

#endif
