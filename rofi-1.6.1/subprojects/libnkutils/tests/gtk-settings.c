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

#include <locale.h>

#include <glib.h>

#include <nkutils-gtk-settings.h>

typedef struct {
    const gchar *key;
    GOptionArg type;
    union {
        gboolean boolean;
        gint64 int64;
        const gchar *string;
    } value;
} NkGtkSettingsTestData;

static const struct {
    const gchar *testpath;
    NkGtkSettingsTestData data;
} _nk_gtk_settings_tests_list[] = {
    {
        .testpath = "/nkutils/gtk-settings/4.0/boolean",
        .data = {
            .key = "gtk-enable-primary-paste",
            .type = G_OPTION_ARG_NONE,
            .value.boolean = TRUE,
        }
    },
    {
        .testpath = "/nkutils/gtk-settings/4.0/uint64",
        .data = {
            .key = "gtk-double-click-time",
            .type = G_OPTION_ARG_INT64,
            .value.int64 = 300,
        }
    },
    {
        .testpath = "/nkutils/gtk-settings/4.0/string",
        .data = {
            .key = "gtk-icon-theme-name",
            .type = G_OPTION_ARG_STRING,
            .value.string = "nothing-like-this-theme",
        }
    },
    {
        .testpath = "/nkutils/gtk-settings/3.0/string",
        .data = {
            .key = "gtk-cursor-theme-name",
            .type = G_OPTION_ARG_STRING,
            .value.string = "gnome",
        }
    },
};

static void
_nk_gtk_settings_tests_func(gconstpointer user_data)
{
    const NkGtkSettingsTestData *data = user_data;
    const gchar *keys[NK_GTK_SETTINGS_NUM_VERSION] = {
        data->key,
        data->key,
    };

    switch ( data->type )
    {
    case G_OPTION_ARG_NONE:
    {
        gboolean ret;
        gboolean value;
        ret = nk_gtk_settings_get_boolean(&value, keys);
        g_assert_true(ret);
        if ( data->value.boolean )
            g_assert_true(value);
        else
            g_assert_false(value);
    }
    break;
    case G_OPTION_ARG_INT64:
    {
        gboolean ret;
        guint64 value;
        ret = nk_gtk_settings_get_uint64(&value, keys);
        g_assert_true(ret);
        g_assert_cmpuint(value, ==, data->value.int64);
    }
    break;
    case G_OPTION_ARG_STRING:
    {
        gboolean ret;
        gchar *value;
        ret = nk_gtk_settings_get_string(&value, keys);
        g_assert_true(ret);
        g_assert_cmpstr(value, ==, data->value.string);
        g_free(value);
    }
    break;
    default:
        g_assert_not_reached();
    }
}

int
main(int argc, char *argv[])
{
    setlocale(LC_ALL, "");

    g_test_init(&argc, &argv, NULL);

    g_test_set_nonfatal_assertions();

    g_setenv("XDG_CONFIG_HOME", SRCDIR G_DIR_SEPARATOR_S "tests", TRUE);
    g_setenv("XDG_DATA_DIRS", "/not/existing/dir", TRUE);

    gsize i;
    for ( i = 0 ; i < G_N_ELEMENTS(_nk_gtk_settings_tests_list) ; ++i )
        g_test_add_data_func(_nk_gtk_settings_tests_list[i].testpath, &_nk_gtk_settings_tests_list[i].data, _nk_gtk_settings_tests_func);

    int ret = g_test_run();
    return ret;
}
