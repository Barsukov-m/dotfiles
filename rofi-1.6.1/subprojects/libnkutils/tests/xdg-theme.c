/*
 * libnkutils/token - Miscellaneous utilities, token module
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

#include <string.h>
#include <locale.h>

#include <glib.h>

#include <nkutils-xdg-theme.h>

#define MAX_THEMES 5

static NkXdgThemeContext *context;
typedef enum {
    TYPE_ICON,
    TYPE_SOUND,
} NkXdgThemeTestType;

typedef struct {
    NkXdgThemeTestType type;
    const gchar *themes[MAX_THEMES];
    const gchar *name;
    const gchar *context;
    gint size;
    gint scale;
    gboolean svg;
    const gchar *profile;
    const gchar *result;
    const gchar *theme_test;
} NkXdgThemeTestData;

static const struct {
    const gchar *testpath;
    NkXdgThemeTestData data;
} _nk_xdg_theme_tests_list[] = {
    {
        .testpath = "/nkutils/xdg-theme/icon/Adwaita/symbolic/found",
        .data = {
            .type = TYPE_ICON,
            .themes = { [0] = "Adwaita" },
            .name = "zoom-in-symbolic",
            .size = 48,
            .scale = 1,
            .svg = TRUE,
            .result = "icons/Adwaita/scalable/actions/zoom-in-symbolic.svg",
            .theme_test = "icons/Adwaita/index.theme",
        }
    },
    {
        .testpath = "/nkutils/xdg-theme/icon/Adwaita/symbolic/not-scalable",
        .data = {
            .type = TYPE_ICON,
            .themes = { [0] = "Adwaita" },
            .name = "application-certificate-symbolic",
            .size = 48,
            .scale = 1,
            .svg = FALSE,
            .result = "icons/Adwaita/48x48/mimetypes/application-certificate-symbolic.symbolic.png",
            .theme_test = "icons/Adwaita/index.theme",
        }
    },
    {
        .testpath = "/nkutils/xdg-theme/icon/Adwaita/symbolic/found-no-symbolic",
        .data = {
            .type = TYPE_ICON,
            .themes = { [0] = "Adwaita" },
            .name = "trophy-gold-symbolic",
            .size = 48,
            .scale = 1,
            .svg = TRUE,
            .result = "icons/Adwaita/48x48/status/trophy-gold.png",
            .theme_test = "icons/Adwaita/index.theme",
        }
    },
    {
        .testpath = "/nkutils/xdg-theme/icon/Adwaita/theme-found/second-choice",
        .data = {
            .type = TYPE_ICON,
            .themes = { [0] = "hopefully-no-theme-has-this-name", [1] = "Adwaita" },
            .name = "trophy-gold",
            .size = 48,
            .scale = 1,
            .svg = TRUE,
            .result = "icons/Adwaita/48x48/status/trophy-gold.png",
            .theme_test = "icons/Adwaita/index.theme",
        }
    },
    {
        .testpath = "/nkutils/xdg-theme/icon/Adwaita/theme-found/fallback",
        .data = {
            .type = TYPE_ICON,
            .themes = { [0] = "Adwaita" },
            .name = "geany",
            .size = 16,
            .scale = 1,
            .svg = FALSE,
            .result = "icons/hicolor/16x16/apps/geany.png",
            .theme_test = "icons/Adwaita/index.theme",
        }
    },
    {
        .testpath = "/nkutils/xdg-theme/icon/Adwaita/context/exist-match/1",
        .data = {
            .type = TYPE_ICON,
            .themes = { [0] = "Adwaita" },
            .name = "network-wireless-signal-ok-symbolic",
            .context = "Status",
            .size = 48,
            .scale = 1,
            .svg = TRUE,
            .result = "icons/Adwaita/scalable/status/network-wireless-signal-ok-symbolic.svg",
            .theme_test = "icons/Adwaita/index.theme",
        }
    },
    {
        .testpath = "/nkutils/xdg-theme/icon/Adwaita/context/exist-no-match",
        .data = {
            .type = TYPE_ICON,
            .themes = { [0] = "Adwaita" },
            .name = "network-wireless-signal-ok-symbolic",
            .context = "Applications",
            .size = 48,
            .scale = 1,
            .svg = TRUE,
            .result = NULL,
            .theme_test = "icons/Adwaita/index.theme",
        }
    },
    {
        .testpath = "/nkutils/xdg-theme/icon/Adwaita/context/exist-match/2",
        .data = {
            .type = TYPE_ICON,
            .themes = { [0] = "Adwaita" },
            .name = "emblem-favorite-symbolic",
            .context = "Emblems",
            .size = 48,
            .scale = 1,
            .svg = TRUE,
            .result = "icons/Adwaita/scalable/emblems/emblem-favorite-symbolic.svg",
            .theme_test = "icons/Adwaita/index.theme",
        }
    },
    {
        .testpath = "/nkutils/xdg-theme/icon/gnome/symbolic/found",
        .data = {
            .type = TYPE_ICON,
            .themes = { [0] = "gnome" },
            .name = "zoom-in-symbolic",
            .size = 48,
            .scale = 1,
            .svg = TRUE,
            .result = "icons/gnome/scalable/actions/zoom-in-symbolic.svg",
            .theme_test = "icons/gnome/index.theme",
        }
    },
    {
        .testpath = "/nkutils/xdg-theme/icon/gnome/symbolic/not-scalable",
        .data = {
            .type = TYPE_ICON,
            .themes = { [0] = "gnome" },
            .name = "zoom-in-symbolic",
            .size = 48,
            .scale = 1,
            .svg = FALSE,
            .result = "icons/gnome/48x48/actions/zoom-in.png",
            .theme_test = "icons/gnome/index.theme",
        }
    },
    {
        .testpath = "/nkutils/xdg-theme/icon/gnome/symbolic/found-no-symbolic",
        .data = {
            .type = TYPE_ICON,
            .themes = { [0] = "gnome" },
            .name = "trophy-gold-symbolic",
            .size = 48,
            .scale = 1,
            .svg = TRUE,
            .result = "icons/gnome/48x48/status/trophy-gold.png",
            .theme_test = "icons/gnome/index.theme",
        }
    },
    {
        .testpath = "/nkutils/xdg-theme/icon/gnome/theme-found/fallback",
        .data = {
            .type = TYPE_ICON,
            .themes = { [0] = "gnome" },
            .name = "geany",
            .size = 16,
            .scale = 1,
            .svg = FALSE,
            .result = "icons/hicolor/16x16/apps/geany.png",
            .theme_test = "icons/gnome/index.theme",
        }
    },
    {
        .testpath = "/nkutils/xdg-theme/icon/gnome/context/exist-match/1",
        .data = {
            .type = TYPE_ICON,
            .themes = { [0] = "gnome" },
            .name = "network-wireless-signal-ok-symbolic",
            .context = "Status",
            .size = 48,
            .scale = 1,
            .svg = TRUE,
            .result = "icons/gnome/scalable/status/network-wireless-signal-ok-symbolic.svg",
            .theme_test = "icons/gnome/index.theme",
        }
    },
    {
        .testpath = "/nkutils/xdg-theme/icon/gnome/context/exist-no-match",
        .data = {
            .type = TYPE_ICON,
            .themes = { [0] = "gnome" },
            .name = "network-wireless-signal-ok-symbolic",
            .context = "Applications",
            .size = 48,
            .scale = 1,
            .svg = TRUE,
            .result = NULL,
            .theme_test = "icons/gnome/index.theme",
        }
    },
    {
        .testpath = "/nkutils/xdg-theme/icon/gnome/context/exist-match/2",
        .data = {
            .type = TYPE_ICON,
            .themes = { [0] = "gnome" },
            .name = "emblem-favorite-symbolic",
            .context = "Emblems",
            .size = 48,
            .scale = 1,
            .svg = TRUE,
            .result = "icons/gnome/scalable/emblems/emblem-favorite-symbolic.svg",
            .theme_test = "icons/gnome/index.theme",
        }
    },
    {
        .testpath = "/nkutils/xdg-theme/icon/no-theme/threshold/found/1",
        .data = {
            .type = TYPE_ICON,
            .name = "geany",
            .size = 18,
            .scale = 1,
            .svg = FALSE,
            .result = "icons/hicolor/16x16/apps/geany.png",
            .theme_test = "icons/hicolor/index.theme",
        }
    },
    {
        .testpath = "/nkutils/xdg-theme/icon/no-theme/threshold/found/2",
        .data = {
            .type = TYPE_ICON,
            .name = "pidgin",
            .size = 18,
            .scale = 1,
            .svg = FALSE,
            .result = "icons/hicolor/16x16/apps/pidgin.png",
            .theme_test = "icons/hicolor/index.theme",
        }
    },
    {
        .testpath = "/nkutils/xdg-theme/icon/no-theme/size/fallback/hicolor/1",
        .data = {
            .type = TYPE_ICON,
            .themes = { [0] = "Adwaita" },
            .name = "pidgin",
            .size = 0,
            .scale = 1,
            .svg = TRUE,
            .result = "icons/hicolor/scalable/apps/pidgin.svg",
            .theme_test = "icons/Adwaita/index.theme",
        }
    },
    {
        .testpath = "/nkutils/xdg-theme/icon/no-theme/size/fallback/pixmaps/1",
        .data = {
            .type = TYPE_ICON,
            .name = "htop",
            .size = 19,
            .scale = 1,
            .svg = FALSE,
            .result = "pixmaps/htop.png",
        }
    },
    {
        .testpath = "/nkutils/xdg-theme/icon/no-theme/size/fallback/pixmaps/2",
        .data = {
            .type = TYPE_ICON,
            .name = "debian-logo",
            .size = 0,
            .scale = 1,
            .svg = FALSE,
            .result = "pixmaps/debian-logo.png",
        }
    },
    {
        .testpath = "/nkutils/xdg-theme/icon/no-theme/size/fallback/home-icons",
        .data = {
            .type = TYPE_ICON,
            .name = "test-icon-home-icons",
            .size = 0,
            .scale = 1,
            .svg = FALSE,
            .result = SRCDIR G_DIR_SEPARATOR_S "tests" G_DIR_SEPARATOR_S "home" G_DIR_SEPARATOR_S ".icons" G_DIR_SEPARATOR_S "test-icon-home-icons.png",
        }
    },
    {
        .testpath = "/nkutils/xdg-theme/icon/Adwaita/size/biggest/fixed",
        .data = {
            .type = TYPE_ICON,
            .themes = { [0] = "Adwaita" },
            .name = "application-certificate-symbolic",
            .size = 0,
            .scale = 1,
            .svg = FALSE,
            .result = "icons/Adwaita/96x96/mimetypes/application-certificate-symbolic.symbolic.png",
            .theme_test = "icons/Adwaita/index.theme",
        }
    },
    {
        .testpath = "/nkutils/xdg-theme/icon/Adwaita/size/biggest/svg",
        .data = {
            .type = TYPE_ICON,
            .themes = { [0] = "Adwaita" },
            .name = "edit-find-symbolic",
            .size = 0,
            .scale = 1,
            .svg = TRUE,
            .result = "icons/Adwaita/scalable/actions/edit-find-symbolic.svg",
            .theme_test = "icons/Adwaita/index.theme",
        }
    },
    {
        .testpath = "/nkutils/xdg-theme/icon/Adwaita/size/best-distance",
        .data = {
            .type = TYPE_ICON,
            .themes = { [0] = "Adwaita" },
            .name = "edit-find",
            .size = 19,
            .scale = 1,
            .svg = FALSE,
            .result = "icons/Adwaita/22x22/actions/edit-find.png",
            .theme_test = "icons/Adwaita/index.theme",
        }
    },
    {
        .testpath = "/nkutils/xdg-theme/icon/gnome/size/biggest/fixed",
        .data = {
            .type = TYPE_ICON,
            .themes = { [0] = "gnome" },
            .name = "edit-find-symbolic",
            .size = 0,
            .scale = 1,
            .svg = FALSE,
            .result = "icons/gnome/96x96/actions/edit-find-symbolic.symbolic.png",
            .theme_test = "icons/gnome/index.theme",
        }
    },
    {
        .testpath = "/nkutils/xdg-theme/icon/gnome/size/biggest/svg",
        .data = {
            .type = TYPE_ICON,
            .themes = { [0] = "gnome" },
            .name = "edit-find-symbolic",
            .size = 0,
            .scale = 1,
            .svg = TRUE,
            .result = "icons/gnome/scalable/actions/edit-find-symbolic.svg",
            .theme_test = "icons/gnome/index.theme",
        }
    },
    {
        .testpath = "/nkutils/xdg-theme/icon/gnome/size/best-distance",
        .data = {
            .type = TYPE_ICON,
            .themes = { [0] = "gnome" },
            .name = "edit-find",
            .size = 19,
            .scale = 1,
            .svg = FALSE,
            .result = "icons/gnome/22x22/actions/edit-find.png",
            .theme_test = "icons/gnome/index.theme",
        }
    },
    {
        .testpath = "/nkutils/xdg-theme/icon/wrong-theme/not-found",
        .data = {
            .type = TYPE_ICON,
            .themes = { [0] = "do-not-exists-hopefully" },
            .name = "nothing-on-earth-will-have-that-name",
            .size = 48,
            .scale = 1,
            .svg = TRUE,
            .result = NULL,
        }
    },
    {
        .testpath = "/nkutils/xdg-theme/icon/no-theme/not-found",
        .data = {
            .type = TYPE_ICON,
            .name = "nothing-on-earth-will-have-that-name",
            .size = 48,
            .scale = 1,
            .svg = TRUE,
            .result = NULL,
        }
    },
    {
        .testpath = "/nkutils/xdg-theme/icon/theme/recursive",
        .data = {
            .type = TYPE_ICON,
            .themes = { [0] = "recursive-theme-test" },
            .name = "test-icon",
            .size = 10,
            .scale = 1,
            .svg = TRUE,
            .result = SRCDIR G_DIR_SEPARATOR_S "tests" G_DIR_SEPARATOR_S "icons" G_DIR_SEPARATOR_S "recursive-theme-test" G_DIR_SEPARATOR_S "test-dir" G_DIR_SEPARATOR_S "test-icon.svg",
        }
    },
    {
        .testpath = "/nkutils/xdg-theme/sound/found/variant",
        .data = {
            .type = TYPE_SOUND,
            .themes = { [0] = "freedesktop" },
            .name = "network-connectivity-established",
            .profile = "stereo",
            .result = "sounds/freedesktop/stereo/network-connectivity-established.oga",
            .theme_test = "sounds/freedesktop/index.theme",
        }
    },
    {
        .testpath = "/nkutils/xdg-theme/sound/fallback/variant",
        .data = {
            .type = TYPE_SOUND,
            .themes = { [0] = "freedesktop" },
            .name = "bell-too-specific",
            .profile = "stereo",
            .result = "sounds/freedesktop/stereo/bell.oga",
            .theme_test = "sounds/freedesktop/index.theme",
        }
    },
    {
        .testpath = "/nkutils/xdg-theme/sound/fallback/freedesktop",
        .data = {
            .type = TYPE_SOUND,
            .themes = { [0] = "non-existing-i-hope" },
            .name = "dialog-information",
            .profile = "stereo",
            .result = "sounds/freedesktop/stereo/dialog-information.oga",
            .theme_test = "sounds/freedesktop/index.theme",
        }
    },
    {
        .testpath = "/nkutils/xdg-theme/sound/fallback/no-index",
        .data = {
            .type = TYPE_SOUND,
            .themes = { [0] = "purple" },
            .name = "logout",
            .profile = "stereo",
            .result = "sounds/purple/logout.wav",
        }
    },
};

static gchar *
_nk_xdg_theme_file_exists(const gchar *file)
{
    if ( g_path_is_absolute(file) )
    {
        if ( ! g_file_test(file, G_FILE_TEST_IS_REGULAR) )
            return NULL;
        return g_strdup(file);
    }

    const gchar * const *system_dirs = g_get_system_data_dirs();
    const gchar * const *system_dir;

    for ( system_dir = system_dirs ; *system_dir != NULL ; ++system_dir )
    {
        gchar *full_path;
        full_path = g_build_filename(*system_dir, file, NULL);
        if ( g_file_test(full_path, G_FILE_TEST_IS_REGULAR) )
            return full_path;
        g_free(full_path);
    }

    return NULL;
}

static void
_nk_xdg_theme_tests_func(gconstpointer user_data)
{
    const NkXdgThemeTestData *data = user_data;

    gchar *theme_test = NULL;
    gchar *expected = NULL;

    if ( data->theme_test != NULL )
    {
        theme_test = _nk_xdg_theme_file_exists(data->theme_test);
        if ( theme_test == NULL )
        {
            g_test_skip("Theme not installed");
            return;
        }
        g_free(theme_test);
    }

    if ( data->result != NULL )
    {
        expected = _nk_xdg_theme_file_exists(data->result);
        if ( expected == NULL )
        {
            g_test_skip("Icon file not installed");
            return;
        }
    }

    gchar *found = NULL;
    switch ( data->type )
    {
    case TYPE_ICON:
        found = nk_xdg_theme_get_icon(context, data->themes, data->context, data->name, data->size, data->scale, data->svg);
    break;
    case TYPE_SOUND:
        found = nk_xdg_theme_get_sound(context, data->themes, data->name, data->profile, NULL);
    break;
    }
    g_assert_cmpstr(found, ==, expected);
    g_free(found);
    g_free(expected);
}

int
main(int argc, char *argv[])
{
    setlocale(LC_ALL, "");

    g_test_init(&argc, &argv, NULL);

    g_test_set_nonfatal_assertions();

    g_setenv("HOME", SRCDIR G_DIR_SEPARATOR_S "tests" G_DIR_SEPARATOR_S "home", TRUE);
    g_setenv("XDG_CONFIG_HOME", SRCDIR G_DIR_SEPARATOR_S "tests", TRUE);
    g_setenv("XDG_DATA_HOME", SRCDIR G_DIR_SEPARATOR_S "tests", TRUE);
    g_setenv("XDG_SESSION_DESKTOP", "", TRUE);
    g_setenv("XDG_CURRENT_DESKTOP", "", TRUE);
    g_setenv("GNOME_DESKTOP_SESSION_ID", "", TRUE);
    g_setenv("KDE_FULL_SESSION", "", TRUE);
    g_setenv("DESKTOP_SESSION", "", TRUE);

    gsize i;
    for ( i = 0 ; i < G_N_ELEMENTS(_nk_xdg_theme_tests_list) ; ++i )
        g_test_add_data_func(_nk_xdg_theme_tests_list[i].testpath, &_nk_xdg_theme_tests_list[i].data, _nk_xdg_theme_tests_func);

    context = nk_xdg_theme_context_new(NULL, NULL);
    int ret = g_test_run();
    nk_xdg_theme_context_free(context);
    return ret;
}
