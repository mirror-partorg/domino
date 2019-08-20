/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * do-price-widget.h
 * Copyright (C) 2015 Ришес Юрий <mir@glekar.com>
 *
 * do-price-widget.c is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * do-price-widget.c is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _DO_PRICE_WIDGET_H_
#define _DO_PRICE_WIDGET_H_

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define DO_TYPE_PRICE_WIDGET             (do_price_widget_get_type ())
#define DO_PRICE_WIDGET(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), DO_TYPE_PRICE_WIDGET, DoPriceWidget))
#define DO_PRICE_WIDGET_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), DO_TYPE_PRICE_WIDGET, DoPriceWidgetClass))
#define DO_IS_PRICE_WIDGET(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), DO_TYPE_PRICE_WIDGET))
#define DO_IS_PRICE_WIDGET_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), DO_TYPE_PRICE_WIDGET))
#define DO_PRICE_WIDGET_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), DO_TYPE_PRICE_WIDGET, DoPriceWidgetClass))

typedef struct _DoPriceWidgetClass DoPriceWidgetClass;
typedef struct _DoPriceWidget DoPriceWidget;
typedef struct _DoPriceWidgetPrivate DoPriceWidgetPrivate;


struct _DoPriceWidgetClass
{
	GtkBoxClass parent_class;
};

struct _DoPriceWidget
{
	GtkBox parent_instance;

	DoPriceWidgetPrivate *priv;
};

GtkWidget* do_price_widget_new (GtkWidget *Header);

GType do_price_widget_get_type (void) G_GNUC_CONST;

G_END_DECLS

#endif /* _DO_PRICE_WIDGET_H_ */

