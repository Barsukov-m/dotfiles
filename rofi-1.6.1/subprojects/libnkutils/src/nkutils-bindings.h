/*
 * libnkutils/bindings - Miscellaneous utilities, bindings module
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

#ifndef __NK_UTILS_BINDINGS_H__
#define __NK_UTILS_BINDINGS_H__

#include <xkbcommon/xkbcommon.h>

typedef struct _NkBindings NkBindings;
typedef struct _NkBindingsSeat NkBindingsSeat;

typedef enum {
    NK_BINDINGS_MODIFIER_SHIFT,
    NK_BINDINGS_MODIFIER_CONTROL,
    NK_BINDINGS_MODIFIER_ALT,
    NK_BINDINGS_MODIFIER_SUPER,
    NK_BINDINGS_MODIFIER_META,
    NK_BINDINGS_MODIFIER_HYPER,
#define NK_BINDINGS_NUM_MODIFIERS 6
} NkBindingsModifiers;

typedef enum {
    NK_BINDINGS_KEY_STATE_PRESS,
    NK_BINDINGS_KEY_STATE_PRESSED,
    NK_BINDINGS_KEY_STATE_RELEASE,
} NkBindingsKeyState;

typedef enum {
    NK_BINDINGS_BUTTON_STATE_PRESS,
    NK_BINDINGS_BUTTON_STATE_RELEASE,
} NkBindingsButtonState;

#define NK_BINDINGS_MODIFIER_MASK(m) (1 << (m))

typedef enum {
    NK_BINDINGS_MOUSE_BUTTON_PRIMARY,
    NK_BINDINGS_MOUSE_BUTTON_SECONDARY,
    NK_BINDINGS_MOUSE_BUTTON_MIDDLE,
    NK_BINDINGS_MOUSE_BUTTON_BACK,
    NK_BINDINGS_MOUSE_BUTTON_FORWARD,
    NK_BINDINGS_MOUSE_BUTTON_EXTRA,
} NkBindingsMouseButton;

typedef enum {
    NK_BINDINGS_SCROLL_AXIS_VERTICAL,
    NK_BINDINGS_SCROLL_AXIS_HORIZONTAL,
#define NK_BINDINGS_SCROLL_NUM_AXIS 2
} NkBindingsScrollAxis;

typedef enum {
    NK_BINDINGS_ERROR_SYNTAX,
    NK_BINDINGS_ERROR_NOTHING,
    NK_BINDINGS_ERROR_ALREADY_REGISTERED,
} NkBindingsError;
#define NK_BINDINGS_ERROR (nk_bindings_error())
GQuark nk_bindings_error(void);

NkBindings *nk_bindings_new(guint64 double_click_delay);
void nk_bindings_free(NkBindings *bindings);

typedef gboolean (*NkBindingsCallback)(guint64 scope, gpointer target, gpointer user_data);
gboolean nk_bindings_add_binding(NkBindings *bindings, guint64 scope, const gchar *string, NkBindingsCallback callback, gpointer user_data, GDestroyNotify notify, GError **error);
void nk_bindings_reset_bindings(NkBindings *bindings);

NkBindingsSeat *nk_bindings_seat_new(NkBindings *bindings, enum xkb_context_flags flags);
void nk_bindings_seat_free(NkBindingsSeat *seat);
void nk_bindings_seat_update_keymap(NkBindingsSeat *seat, struct xkb_keymap *keymap, struct xkb_state *state);

struct xkb_context *nk_bindings_seat_get_context(NkBindingsSeat *seat);

gchar *nk_bindings_seat_handle_key(NkBindingsSeat *seat, gpointer target, xkb_keycode_t key, NkBindingsKeyState state);
gchar *nk_bindings_seat_handle_key_with_modmask(NkBindingsSeat *self, gpointer target, xkb_mod_mask_t modmask, xkb_keycode_t keycode, NkBindingsKeyState state);
gboolean nk_bindings_seat_handle_button(NkBindingsSeat *seat, gpointer target, NkBindingsMouseButton button, NkBindingsButtonState state, guint64 timestamp);
gboolean nk_bindings_seat_handle_scroll(NkBindingsSeat *seat, gpointer target, NkBindingsScrollAxis axis, gint32 steps);
void nk_bindings_seat_update_mask(NkBindingsSeat *seat, gpointer target, xkb_mod_mask_t depressed_mods, xkb_mod_mask_t latched_mods, xkb_mod_mask_t locked_mods, xkb_layout_index_t depressed_layout, xkb_layout_index_t latched_layout, xkb_layout_index_t locked_layout);
void nk_bindings_seat_reset(NkBindingsSeat *seat);

#endif /* __NK_UTILS_BINDINGS_H__ */
