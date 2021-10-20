/*
 * libnkutils/xdg-de - Miscellaneous utilities, XDG DE module
 *
 * Copyright © 2011-2017 Quentin "Sardem FF7" Glidic
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#ifdef G_LOG_DOMAIN
#undef G_LOG_DOMAIN
#endif /* G_LOG_DOMAIN */
#define G_LOG_DOMAIN "libnkutils-gtk-settings"

#include <glib.h>

#include "nkutils-gtk-settings.h"

typedef struct {
    const gchar *path;
    const gchar *group;
} NkGtkSettingsFile;

static const NkGtkSettingsFile _nk_gtk_settings_files[NK_GTK_SETTINGS_NUM_VERSION] = {
        {
            .path = "gtk-3.0" G_DIR_SEPARATOR_S "settings.ini",
            .group = "Settings",
        },
        {
            .path = "gtk-4.0" G_DIR_SEPARATOR_S "settings.ini",
            .group = "Settings",
        },
};

typedef gboolean (*NkGtkSettingsGetter)(gpointer value, GKeyFile *settings, const gchar *group, const gchar *key);
static gboolean
_nk_gtk_settings_try_dir(gpointer value, const gchar *keys[NK_GTK_SETTINGS_NUM_VERSION], NkGtkSettingsGetter getter, const gchar *dir)
{
    gboolean found = FALSE;
    gssize i;
    for ( i = NK_GTK_SETTINGS_NUM_VERSION - 1 ; ( ! found ) && ( i >= 0 ) ; --i )
    {
        const NkGtkSettingsFile *file = &_nk_gtk_settings_files[i];
        gchar *path;
        GKeyFile *settings;

        path = g_build_filename(dir, file->path, NULL);
        settings = g_key_file_new();

        if ( g_key_file_load_from_file(settings, path, G_KEY_FILE_NONE, NULL)
             && g_key_file_has_group(settings, file->group)
             && g_key_file_has_key(settings, file->group, keys[i], NULL) )
            found = getter(value, settings, file->group, keys[i]);
        g_key_file_free(settings);
        g_free(path);
    }

    return found;
}

static gboolean
_nk_gtk_settings_get(gpointer value, const gchar *keys[NK_GTK_SETTINGS_NUM_VERSION], NkGtkSettingsGetter getter)
{
    if ( _nk_gtk_settings_try_dir(value, keys, getter, g_get_user_config_dir()) )
        return TRUE;
    const gchar * const *dir;
    dir = g_get_system_config_dirs();
    for ( ; *dir != NULL ; ++dir )
    {
        if ( _nk_gtk_settings_try_dir(value, keys, getter, *dir) )
            return TRUE;
    }
    return _nk_gtk_settings_try_dir(value, keys, getter, SYSCONFDIR);
}

static gboolean
_nk_gtk_settings_getter_boolean(gpointer ret_, GKeyFile *settings, const gchar *group, const gchar *key)
{
    gboolean *ret = ret_, value;
    GError *error = NULL;

    value = g_key_file_get_boolean(settings, group, key, &error);
    if ( error == NULL )
        *ret = value;
    else
        g_error_free(error);

    return ( error == NULL );
}

gboolean
nk_gtk_settings_get_boolean(gboolean *value, const gchar *keys[NK_GTK_SETTINGS_NUM_VERSION])
{
    return _nk_gtk_settings_get(value, keys, _nk_gtk_settings_getter_boolean);
}

static gboolean
_nk_gtk_settings_getter_uint64(gpointer ret_, GKeyFile *settings, const gchar *group, const gchar *key)
{
    guint64 *ret = ret_, value;
    GError *error = NULL;

    value = g_key_file_get_uint64(settings, group, key, &error);
    if ( error == NULL )
        *ret = value;
    else
        g_error_free(error);

    return ( error == NULL );
}

gboolean
nk_gtk_settings_get_uint64(guint64 *value, const gchar *keys[NK_GTK_SETTINGS_NUM_VERSION])
{
    return _nk_gtk_settings_get(value, keys, _nk_gtk_settings_getter_uint64);
}

static gboolean
_nk_gtk_settings_getter_string(gpointer ret_, GKeyFile *settings, const gchar *group, const gchar *key)
{
    gchar **ret = ret_, *value;
    GError *error = NULL;

    value = g_key_file_get_string(settings, group, key, &error);
    if ( error == NULL )
        *ret = value;
    else
        g_error_free(error);

    return ( error == NULL );
}

gboolean
nk_gtk_settings_get_string(gchar **value, const gchar *keys[NK_GTK_SETTINGS_NUM_VERSION])
{
    return _nk_gtk_settings_get(value, keys, _nk_gtk_settings_getter_string);
}
