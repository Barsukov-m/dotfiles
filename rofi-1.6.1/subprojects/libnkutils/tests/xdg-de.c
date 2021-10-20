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

#include <nkutils-xdg-de.h>

typedef struct {
    const gchar *var;
    const gchar *value;
    NkXdgDe de;
} NkXdgDeTestData;

static const struct {
    const gchar *testpath;
    NkXdgDeTestData data;
} _nk_xdg_de_tests_list[] = {
    {
        .testpath = "/nkutils/xdg-de/XDG_CURRENT_DESKTOP/generic",
        .data = {
            .var = "XDG_CURRENT_DESKTOP",
            .value = "X-Generic",
            .de = NK_XDG_DE_NONE,
        }
    },
    {
        .testpath = "/nkutils/xdg-de/XDG_CURRENT_DESKTOP/gnome/case-match",
        .data = {
            .var = "XDG_CURRENT_DESKTOP",
            .value = "GNOME",
            .de = NK_XDG_DE_GNOME,
        }
    },
    {
        .testpath = "/nkutils/xdg-de/XDG_CURRENT_DESKTOP/gnome/multi-names",
        .data = {
            .var = "XDG_CURRENT_DESKTOP",
            .value = "GNOME:GNOME-Flashback",
            .de = NK_XDG_DE_GNOME,
        }
    },
    {
        .testpath = "/nkutils/xdg-de/XDG_CURRENT_DESKTOP/gnome/case-not-match",
        .data = {
            .var = "XDG_CURRENT_DESKTOP",
            .value = "gnome",
            .de = NK_XDG_DE_NONE,
        }
    },
    {
        .testpath = "/nkutils/xdg-de/XDG_CURRENT_DESKTOP/kde",
        .data = {
            .var = "XDG_CURRENT_DESKTOP",
            .value = "KDE",
            .de = NK_XDG_DE_KDE,
        }
    },
    {
        .testpath = "/nkutils/xdg-de/XDG_CURRENT_DESKTOP/unknown",
        .data = {
            .var = "XDG_CURRENT_DESKTOP",
            .value = "WeirdDE",
            .de = NK_XDG_DE_NONE,
        }
    },
    {
        .testpath = "/nkutils/xdg-de/XDG_SESSION_DESKTOP/generic",
        .data = {
            .var = "XDG_SESSION_DESKTOP",
            .value = "generic",
            .de = NK_XDG_DE_NONE,
        }
    },
    {
        .testpath = "/nkutils/xdg-de/XDG_SESSION_DESKTOP/gnome/case-match",
        .data = {
            .var = "XDG_SESSION_DESKTOP",
            .value = "GNOME",
            .de = NK_XDG_DE_GNOME,
        }
    },
    {
        .testpath = "/nkutils/xdg-de/XDG_SESSION_DESKTOP/gnome/case-not-match",
        .data = {
            .var = "XDG_SESSION_DESKTOP",
            .value = "gnome",
            .de = NK_XDG_DE_GNOME,
        }
    },
    {
        .testpath = "/nkutils/xdg-de/XDG_SESSION_DESKTOP/gnome/flashback",
        .data = {
            .var = "XDG_SESSION_DESKTOP",
            .value = "GNOME-Flashback",
            .de = NK_XDG_DE_GNOME,
        }
    },
    {
        .testpath = "/nkutils/xdg-de/XDG_SESSION_DESKTOP/kde",
        .data = {
            .var = "XDG_SESSION_DESKTOP",
            .value = "KDE",
            .de = NK_XDG_DE_KDE,
        }
    },
    {
        .testpath = "/nkutils/xdg-de/XDG_SESSION_DESKTOP/unknown",
        .data = {
            .var = "XDG_SESSION_DESKTOP",
            .value = "WeirdDE",
            .de = NK_XDG_DE_NONE,
        }
    },
    {
        .testpath = "/nkutils/xdg-de/GNOME_DESKTOP_SESSION_ID",
        .data = {
            .var = "GNOME_DESKTOP_SESSION_ID",
            .value = "1",
            .de = NK_XDG_DE_GNOME,
        }
    },
    {
        .testpath = "/nkutils/xdg-de/KDE_FULL_SESSION",
        .data = {
            .var = "KDE_FULL_SESSION",
            .value = "1",
            .de = NK_XDG_DE_KDE,
        }
    },
    {
        .testpath = "/nkutils/xdg-de/DESKTOP_SESSION/generic",
        .data = {
            .var = "DESKTOP_SESSION",
            .value = "generic",
            .de = NK_XDG_DE_NONE,
        }
    },
    {
        .testpath = "/nkutils/xdg-de/DESKTOP_SESSION/gnome/case-match",
        .data = {
            .var = "DESKTOP_SESSION",
            .value = "gnome",
            .de = NK_XDG_DE_GNOME,
        }
    },
    {
        .testpath = "/nkutils/xdg-de/DESKTOP_SESSION/gnome/case-not-match",
        .data = {
            .var = "DESKTOP_SESSION",
            .value = "GNOME",
            .de = NK_XDG_DE_NONE,
        }
    },
    {
        .testpath = "/nkutils/xdg-de/DESKTOP_SESSION/kde",
        .data = {
            .var = "DESKTOP_SESSION",
            .value = "kde",
            .de = NK_XDG_DE_KDE,
        }
    },
    {
        .testpath = "/nkutils/xdg-de/DESKTOP_SESSION/unkown",
        .data = {
            .var = "DESKTOP_SESSION",
            .value = "weirdde",
            .de = NK_XDG_DE_NONE,
        }
    },
};

static void
_nk_xdg_de_tests_func(gconstpointer user_data)
{
    const NkXdgDeTestData *data = user_data;

    if ( g_test_subprocess() )
    {
        NkXdgDe de;
        g_setenv(data->var, data->value, TRUE);
        de = nk_xdg_de_detect();
        g_assert_cmpint(de, ==, data->de);
        return;
    }
    g_test_trap_subprocess(NULL, 0, G_TEST_SUBPROCESS_INHERIT_STDOUT | G_TEST_SUBPROCESS_INHERIT_STDERR);
    g_test_trap_assert_passed();
}

int
main(int argc, char *argv[])
{
    setlocale(LC_ALL, "");

    g_test_init(&argc, &argv, NULL);

    g_test_set_nonfatal_assertions();

    g_setenv("XDG_SESSION_DESKTOP", "", TRUE);
    g_setenv("XDG_CURRENT_DESKTOP", "", TRUE);
    g_setenv("GNOME_DESKTOP_SESSION_ID", "", TRUE);
    g_setenv("KDE_FULL_SESSION", "", TRUE);
    g_setenv("DESKTOP_SESSION", "", TRUE);

    gsize i;
    for ( i = 0 ; i < G_N_ELEMENTS(_nk_xdg_de_tests_list) ; ++i )
        g_test_add_data_func(_nk_xdg_de_tests_list[i].testpath, &_nk_xdg_de_tests_list[i].data, _nk_xdg_de_tests_func);

    int ret = g_test_run();
    return ret;
}
