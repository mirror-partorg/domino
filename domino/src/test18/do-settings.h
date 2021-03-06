/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * do-settings.h
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

#ifndef _DO_SETTINGS_H_
#define _DO_SETTINGS_H_

#include <glib-object.h>

G_BEGIN_DECLS

#define DO_TYPE_SETTINGS             (do_settings_get_type ())
#define DO_SETTINGS(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), DO_TYPE_SETTINGS, DoSettings))
#define DO_SETTINGS_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), DO_TYPE_SETTINGS, DoSettingsClass))
#define DO_IS_SETTINGS(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), DO_TYPE_SETTINGS))
#define DO_IS_SETTINGS_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), DO_TYPE_SETTINGS))
#define DO_SETTINGS_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), DO_TYPE_SETTINGS, DoSettingsClass))

typedef struct _DoSettingsClass DoSettingsClass;
typedef struct _DoSettings DoSettings;
typedef struct _DoSettingsPrivate DoSettingsPrivate;

struct _DoSettingsClass
{
	GObjectClass parent_class;
};

struct _DoSettings
{
	GObject parent_instance;

	DoSettingsPrivate *priv;
};

GType do_settings_get_type (void) G_GNUC_CONST;
DoSettings * do_settings_new (const gchar *application_name);
gchar *do_settings_get_string (const gchar *section, const gchar *name, const gchar *default_value);
gint do_settings_get_int (const gchar *section, const gchar *name, gint default_value);
gboolean do_settings_get_boolean (const gchar *section, const gchar *name, gboolean default_value);
gdouble do_settings_get_double (const gchar *section, const gchar *name, gdouble default_value);
void do_settings_set_string (const gchar *section, const gchar *name, const gchar *value);
void do_settings_set_int (const gchar *section, const gchar *name, gint value);
void do_settings_set_double (const gchar *section, const gchar *name, gdouble value);
void do_settings_set_boolean (const gchar *section, const gchar *name, gboolean value);

G_END_DECLS

#endif /* _DO_SETTINGS_H_ */

