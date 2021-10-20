/*
 * libnkutils/xdg-theme - Miscellaneous utilities, XDG Theme module
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

#ifndef __NK_UTILS_XDG_THEME_H__
#define __NK_UTILS_XDG_THEME_H__

typedef struct _NkXdgThemeContext NkXdgThemeContext;

NkXdgThemeContext *nk_xdg_theme_context_new(const gchar * const *icon_fallback_themes, const gchar * const *sound_fallback_themes);
void nk_xdg_theme_context_free(NkXdgThemeContext *context);

void nk_xdg_theme_preload_themes_icon(NkXdgThemeContext *context, const gchar * const *theme_names);
void nk_xdg_theme_preload_themes_sound(NkXdgThemeContext *context, const gchar * const *theme_names);

gchar *nk_xdg_theme_get_icon(NkXdgThemeContext *context, const gchar * const *themes, const gchar *context_name, const gchar *name, gint size, gint scale, gboolean svg);
gchar *nk_xdg_theme_get_sound(NkXdgThemeContext *context, const gchar * const *themes, const gchar *name, const gchar *profile, const gchar *locale);

#endif /* __NK_UTILS_XDG_THEME_H__ */
