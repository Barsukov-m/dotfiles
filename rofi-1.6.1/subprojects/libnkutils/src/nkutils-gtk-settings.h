/*
 * libnkutils/xdg-de - Miscellaneous utilities, GTK+ settings module
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

#ifndef __NK_UTILS_GTK_SETTINGS_H__
#define __NK_UTILS_GTK_SETTINGS_H__

typedef enum {
    NK_GTK_SETTINGS_VERSION_3,
    NK_GTK_SETTINGS_VERSION_4,
    NK_GTK_SETTINGS_NUM_VERSION,
} NkGtkSettingsVersion;

gboolean nk_gtk_settings_get_boolean(gboolean *value, const gchar *keys[NK_GTK_SETTINGS_NUM_VERSION]);
gboolean nk_gtk_settings_get_uint64(guint64 *value, const gchar *keys[NK_GTK_SETTINGS_NUM_VERSION]);
gboolean nk_gtk_settings_get_string(gchar **value, const gchar *keys[NK_GTK_SETTINGS_NUM_VERSION]);

#endif /* __NK_UTILS_GTK_SETTINGS_H__ */
