/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * do-window.h
 * Copyright (C) 2015 Ришес Юрий <mir@glekar.com>
 *
 * price is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * price is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _DO_WINDOW_H_
#define _DO_WINDOW_H_

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define DO_TYPE_WINDOW             (do_window_get_type ())
#define DO_WINDOW(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), DO_TYPE_WINDOW, DoWindow))
#define DO_WINDOW_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), DO_TYPE_WINDOW, DoWindowClass))
#define DO_IS_WINDOW(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), DO_TYPE_WINDOW))
#define DO_IS_WINDOW_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), DO_TYPE_WINDOW))
#define DO_WINDOW_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), DO_TYPE_WINDOW, DoWindowClass))

typedef struct _DoWindowClass DoWindowClass;
typedef struct _DoWindow DoWindow;
typedef struct _DoWindowPrivate DoWindowPrivate;


struct _DoWindowClass
{
	GtkWindowClass parent_class;
};

struct _DoWindow
{
	GtkWindow parent_instance;

	DoWindowPrivate *priv;
};

GType do_window_get_type (void) G_GNUC_CONST;
GtkWidget * do_window_new (const gchar *name);

G_END_DECLS

#endif /* _DO_WINDOW_H_ */

