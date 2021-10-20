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
#define G_LOG_DOMAIN "libnkutils-xdg-de"

#include <string.h>

#include <glib.h>
#include <glib/gprintf.h>

#include "nkutils-enum.h"
#include "nkutils-xdg-de.h"

static const gchar * const _nk_xdg_de_current_desktop_generic_names[] = {
    "generic",
    NULL
};

static const gchar * const _nk_xdg_de_current_desktop_gnome_names[] = {
    "GNOME",
    "GNOME-Flashback",
    NULL
};

static const gchar * const _nk_xdg_de_current_desktop_kde_names[] = {
    "KDE",
    NULL
};

static const gchar * const _nk_xdg_de_current_desktop_i3_names[] = {
    "i3",
    NULL
};

static const struct {
    const gchar * const * const names;
    gsize size;
} _nk_xdg_de_current_desktop_names[] = {
    [NK_XDG_DE_NONE] = { .names = _nk_xdg_de_current_desktop_generic_names, .size = G_N_ELEMENTS(_nk_xdg_de_current_desktop_generic_names), },
    [NK_XDG_DE_GNOME] = { .names = _nk_xdg_de_current_desktop_gnome_names, .size = G_N_ELEMENTS(_nk_xdg_de_current_desktop_gnome_names), },
    [NK_XDG_DE_KDE] = { .names = _nk_xdg_de_current_desktop_kde_names, .size = G_N_ELEMENTS(_nk_xdg_de_current_desktop_kde_names), },
    [NK_XDG_DE_I3] = { .names = _nk_xdg_de_current_desktop_i3_names, .size = G_N_ELEMENTS(_nk_xdg_de_current_desktop_i3_names), },
};

static const gchar * const _nk_xdg_de_session_desktop_names[] = {
    [NK_XDG_DE_NONE] = "X-Generic",
    [NK_XDG_DE_GNOME] = "GNOME",
    [NK_XDG_DE_KDE] = "KDE",
    [NK_XDG_DE_I3] = "i3",
};

static const gchar * const _nk_xdg_de_desktop_session_names[] = {
    [NK_XDG_DE_NONE] = "generic",
    [NK_XDG_DE_GNOME] = "gnome",
    [NK_XDG_DE_KDE] = "kde",
    [NK_XDG_DE_I3] = "i3",
};

NkXdgDe
nk_xdg_de_detect(void)
{
    static NkXdgDe _nk_xdg_de = NK_XDG_DE_NONE - 1;
    if ( _nk_xdg_de != (NkXdgDe) ( NK_XDG_DE_NONE - 1 ) )
        return _nk_xdg_de;

    const gchar *var;
    guint64 value;

    var = g_getenv("XDG_CURRENT_DESKTOP");
    if ( var != NULL )
    {
        const gchar *s, *e = var + strlen(var);
        gsize i;
        for ( ; var < e ; var = g_utf8_next_char(s) )
        {
            s = g_utf8_strchr(var, e - var, ':');
            if ( s == NULL )
                s = e;
            gsize l = s - var + 1;
            gchar *val = g_newa(gchar, l);
            g_snprintf(val, l, "%s", var);
            for ( i = 0 ; i < G_N_ELEMENTS(_nk_xdg_de_current_desktop_names) ; ++i )
            {
                if ( nk_enum_parse(val, _nk_xdg_de_current_desktop_names[i].names, _nk_xdg_de_current_desktop_names[i].size, FALSE, FALSE, &value) )
                    _nk_xdg_de = i;
            }
        }
        if ( _nk_xdg_de != (NkXdgDe) ( NK_XDG_DE_NONE - 1 ) )
            return _nk_xdg_de;
    }

    var = g_getenv("XDG_SESSION_DESKTOP");
    if ( ( var != NULL ) && nk_enum_parse(var, _nk_xdg_de_session_desktop_names, G_N_ELEMENTS(_nk_xdg_de_session_desktop_names), TRUE, TRUE, &value) )
    {
        _nk_xdg_de = value;
        return _nk_xdg_de;
    }

    var = g_getenv("GNOME_DESKTOP_SESSION_ID");
    if ( ( var != NULL ) && ( *var != '\0' ) )
    {
        _nk_xdg_de = NK_XDG_DE_GNOME;
        return _nk_xdg_de;
    }

    var = g_getenv("KDE_FULL_SESSION");
    if ( ( var != NULL ) && ( *var != '\0' ) )
    {
        _nk_xdg_de = NK_XDG_DE_KDE;
        return _nk_xdg_de;
    }

    var = g_getenv("DESKTOP_SESSION");
    if ( ( var != NULL ) && nk_enum_parse(var, _nk_xdg_de_desktop_session_names, G_N_ELEMENTS(_nk_xdg_de_desktop_session_names), FALSE, FALSE, &value) )
    {
        _nk_xdg_de = value;
        return _nk_xdg_de;
    }

    _nk_xdg_de = NK_XDG_DE_NONE;
    return _nk_xdg_de;
}
