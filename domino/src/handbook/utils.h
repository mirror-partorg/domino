#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <glib.h>
#include "messages.h"

#define MODULES_INIT(a,b) modules_init(a,b)
#define BUILDER_GET_WIDGET(widget, builder, name) {widget=GTK_WIDGET(gtk_builder_get_object(builder, name)); if ( !widget) g_warning ("Failed to fetch object \"%s\"\n", name);}

G_BEGIN_DECLS


gboolean modules_init(int argc, char *argv[]);


G_END_DECLS


#endif // UTILS_H_INCLUDED
