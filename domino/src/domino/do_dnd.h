
#ifndef DO_DND_H
#define DO_DND_H

#include <glib.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

/* Drag & Drop target names. */
#define DO_DND_TEXT_TYPE              "text/plain"
#define DO_DND_URL_TYPE               "text/uri-list"
#define DO_DND_PRODUCT_TYPE           "_DO_PRODUCT_"
#define DO_DND_STRING_TYPE            "STRING"

#define DRAG_ICON_LAYOUT_BORDER		2
#define DRAG_ICON_ICON_SPACING		DRAG_ICON_LAYOUT_BORDER * 2
#define DRAG_ICON_MAX_WIDTH_CHARS	32

G_END_DECLS

#endif
