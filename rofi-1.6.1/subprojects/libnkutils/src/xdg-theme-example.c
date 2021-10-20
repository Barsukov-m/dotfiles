/*
 * libnkutils/xdg-theme - Miscellaneous utilities, xdg-theme module
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

#ifdef G_LOG_DOMAIN
#undef G_LOG_DOMAIN
#endif /* G_LOG_DOMAIN */
#define G_LOG_DOMAIN "nk-xdg-theme-lookup"

#include <glib.h>

#include "nkutils-xdg-theme.h"

int
main(int argc, char *argv[])
{
    setlocale(LC_ALL, "");

    gint size = 0;
    GOptionEntry entries[] =
    {
        { "size", 's', 0, G_OPTION_ARG_INT,     &size,  "Icon size", NULL },
        { .long_name = NULL }
    };

    GError *error = NULL;
    GOptionContext *option_context;

    option_context = g_option_context_new("<icon name> [<theme name>…] - example utilise for libnkutils xdg-theme module");
    g_option_context_add_main_entries(option_context, entries, NULL);
    if ( ! g_option_context_parse(option_context, &argc, &argv, &error) )
    {
        g_warning("Option parsing failed: %s\n", error->message);
        return 1;
    }
    g_option_context_free(option_context);

    if ( argc < 2 )
    {
        g_warning("You must provide an icon name");
        return 1;
    }

    NkXdgThemeContext *context;
    const gchar * const *themes = (const gchar * const *) argv + 2;
    gchar *icon;

    context = nk_xdg_theme_context_new(NULL, NULL);
    icon = nk_xdg_theme_get_icon(context, themes, NULL, argv[1], size, 1, TRUE);

    g_print("%s\n", icon);

    g_free(icon);
    nk_xdg_theme_context_free(context);

    return 0;
}
