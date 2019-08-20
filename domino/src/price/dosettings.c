/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * do-settings.c
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

#include "dosettings.h"
#include "config.h"

#define COMPANY_NAME "glekar"

static DoSettings *settings = NULL;


struct _DoSettingsPrivate
{
	 gchar *filename;
	 gchar *application_name;
	 GKeyFile *key_file;
};

enum
{
	PROP_0,

	PROP_APPLICATION_NAME
};


G_DEFINE_TYPE (DoSettings, do_settings, G_TYPE_OBJECT);

static void
do_settings_file_init (DoSettings *do_settings)
{
	gchar *filename;
    gchar *path;
	filename = g_strdup_printf("%s.ini", do_settings->priv->application_name);
	g_free (do_settings->priv->filename);
	path = g_build_filename (g_get_user_config_dir(), do_settings->priv->application_name, NULL);
	do_settings->priv->filename = g_build_filename (path, filename, NULL);
	g_mkdir_with_parents(path, 0700);
	g_free (path);
	g_free (filename);

	do_settings->priv->key_file = g_key_file_new ();
    g_key_file_load_from_file(do_settings->priv->key_file, do_settings->priv->filename, 0, NULL);

}
static void
do_settings_init (DoSettings *do_settings)
{
	do_settings->priv = G_TYPE_INSTANCE_GET_PRIVATE (do_settings, DO_TYPE_SETTINGS, DoSettingsPrivate);

}
static gboolean
do_settings_save (DoSettings *settings)
{

	DoSettingsPrivate *priv = settings->priv;
	if ( priv->key_file && priv->filename ) {
        //MESSAGE("save");
        GError *error=NULL;

		if ( !g_key_file_save_to_file (priv->key_file, priv->filename, &error) ) {
            if ( error ) {
                g_log (priv->application_name, G_LOG_LEVEL_WARNING, "%s", error->message);
                g_error_free(error);
            }
            else
                g_log (priv->application_name, G_LOG_LEVEL_WARNING, "Невозможно сохранить файл '%s'", priv->filename);
            return FALSE;
		}
		return TRUE;
    }
	return FALSE;
}

static void
do_settings_finalize (GObject *object)
{
	DoSettingsPrivate *priv = DO_SETTINGS (object)->priv;

	if ( priv->key_file )
	{
		do_settings_save (DO_SETTINGS (object));
		g_key_file_free (priv->key_file);
	}
	g_free (priv->filename);
	g_free (priv->application_name);

	G_OBJECT_CLASS (do_settings_parent_class)->finalize (object);
}

static void
do_settings_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	g_return_if_fail (DO_IS_SETTINGS (object));
	DoSettingsPrivate *priv = DO_SETTINGS (object)->priv;

	switch (prop_id)
	{
	case PROP_APPLICATION_NAME:
		do_settings_save (DO_SETTINGS (object));
		g_free (priv->application_name);
		if ( !g_value_get_string(value) )
            priv->application_name = g_strdup (COMPANY_NAME);
		else
            priv->application_name = g_value_dup_string (value);
		do_settings_file_init (DO_SETTINGS (object));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
do_settings_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	g_return_if_fail (DO_IS_SETTINGS (object));
	DoSettingsPrivate *priv = DO_SETTINGS (object)->priv;

	switch (prop_id)
	{
	case PROP_APPLICATION_NAME:
		g_value_set_string (value, priv->application_name);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
do_settings_class_init (DoSettingsClass *klass)
{
	GObjectClass* object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (DoSettingsPrivate));

	object_class->finalize = do_settings_finalize;
	object_class->set_property = do_settings_set_property;
	object_class->get_property = do_settings_get_property;

	g_object_class_install_property (object_class,
	                                 PROP_APPLICATION_NAME,
	                                 g_param_spec_string ("application_name",
	                                                      "application-name",
	                                                      "name of application",
	                                                      "glekar",
	                                                      G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
}


DoSettings *
do_settings_new (const gchar *application_name)
{
	settings = g_object_new (DO_TYPE_SETTINGS,
	                         "application-name", application_name,
	                         NULL);
	return settings;
}

gchar *
do_settings_get_string (const gchar *section, const gchar *name, const gchar *default_value)
{
	if ( !settings ) do_settings_new (NULL);
	DoSettingsPrivate *priv = settings->priv;

	if ( g_key_file_has_key (priv->key_file, section, name, NULL) )
		return g_key_file_get_string (priv->key_file, section, name, NULL);

	return default_value ? g_strdup (default_value) : NULL;
}

gint
do_settings_get_int (const gchar *section, const gchar *name, gint default_value)
{
	if ( !settings ) do_settings_new (NULL);
	DoSettingsPrivate *priv = settings->priv;

	if ( g_key_file_has_key (priv->key_file, section, name, NULL) )
		return g_key_file_get_integer (priv->key_file, section, name, NULL);

	return default_value;
}

gboolean
do_settings_get_boolean (const gchar *section, const gchar *name, gboolean default_value)
{
	if ( !settings ) do_settings_new (NULL);
	DoSettingsPrivate *priv = settings->priv;
	if ( g_key_file_has_key (priv->key_file, section, name, NULL) )
		return g_key_file_get_boolean (priv->key_file, section, name, NULL);

	return default_value;
}

gdouble
do_settings_get_float (const gchar *section, const gchar *name, gdouble default_value)
{
	if ( !settings ) do_settings_new (NULL);
	DoSettingsPrivate *priv = settings->priv;
	if ( g_key_file_has_key (priv->key_file, section, name, NULL) )
		return g_key_file_get_double (priv->key_file, section, name, NULL);

	return default_value;
}

void
do_settings_set_string (const gchar *section, const gchar *name, const gchar *value)
{
	if ( !settings ) do_settings_new (NULL);
	DoSettingsPrivate *priv = settings->priv;
	g_key_file_set_string (priv->key_file, section, name, value);
	do_settings_save (settings);
}

void
do_settings_set_int (const gchar *section, const gchar *name, gint value)
{
	if ( !settings ) do_settings_new (NULL);
	DoSettingsPrivate *priv = settings->priv;
	g_key_file_set_integer (priv->key_file, section, name, value);
	do_settings_save (settings);
}

void
do_settings_set_double (const gchar *section, const gchar *name, gdouble value)
{
	if ( !settings ) do_settings_new (NULL);
	DoSettingsPrivate *priv = settings->priv;
	g_key_file_set_double (priv->key_file, section, name, value);
	do_settings_save (settings);
}

void
do_settings_set_boolean (const gchar *section, const gchar *name, gboolean value)
{
	if ( !settings ) do_settings_new (NULL);
	DoSettingsPrivate *priv = settings->priv;
	g_key_file_set_boolean (priv->key_file, section, name, value);
	do_settings_save (settings);
}

