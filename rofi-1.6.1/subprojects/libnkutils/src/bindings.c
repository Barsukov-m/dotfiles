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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#ifdef G_LOG_DOMAIN
#undef G_LOG_DOMAIN
#endif /* G_LOG_DOMAIN */
#define G_LOG_DOMAIN "libnkutils-bindings"

#include <errno.h>
#include <locale.h>
#include <string.h>

#include <glib.h>

#include <xkbcommon/xkbcommon.h>
#ifdef NK_XKBCOMMON_HAS_COMPOSE
#include <xkbcommon/xkbcommon-compose.h>
#endif /* NK_XKBCOMMON_HAS_COMPOSE */

#include "nkutils-gtk-settings.h"
#include "nkutils-xdg-de.h"
#include "nkutils-enum.h"
#include "nkutils-bindings.h"

#define NK_BINDINGS_DEFAULT_DOUBLE_CLICK_DELAY 200

#define NK_BINDINGS_MAX_ALIASES 4 /* For Alt */

#define _NK_VALUE_TO_BINDING(mask, value) ((guint64)((((guint64)(mask)) << 32) | (guint32)(value)))
#define NK_KEYCODE_TO_BINDING(mask, keycode) _NK_VALUE_TO_BINDING(mask, keycode)
#define NK_KEYSYM_TO_BINDING(mask, keysym) _NK_VALUE_TO_BINDING(mask, keysym)
#define NK_BUTTON_TO_BINDING(mask, button) _NK_VALUE_TO_BINDING(mask, button)
#define NK_SCROLL_TO_BINDING(mask, axis, step) _NK_VALUE_TO_BINDING(mask, (axis << 1) + ( (step < 0) ? 0 : 1 ))

struct _NkBindings {
    guint64 double_click_delay;
    GList *scopes;
    GList *seats;
};

struct _NkBindingsSeat {
    NkBindings *bindings;
    GList *link;
    struct xkb_context *context;
    struct xkb_keymap  *keymap;
    struct xkb_state   *state;
    xkb_mod_index_t     modifiers[NK_BINDINGS_NUM_MODIFIERS][NK_BINDINGS_MAX_ALIASES + 1];
#ifdef NK_XKBCOMMON_HAS_COMPOSE
    struct {
        struct xkb_compose_table *table;
        struct xkb_compose_state *state;
    } compose;
#endif /* NK_XKBCOMMON_HAS_COMPOSE */
    GList *on_release;
    GHashTable *last_timestamps;
};

typedef struct {
    guint64 id;
    GHashTable *keycodes;
    GHashTable *keysyms;
    GHashTable *buttons;
    GHashTable *scroll;
} NkBindingsScope;

typedef struct {
    NkBindingsCallback callback;
    gpointer user_data;
    GDestroyNotify notify;
} NkBindingsBindingBase;

typedef struct {
    NkBindingsBindingBase base;
} NkBindingsBindingPress;

typedef struct {
    NkBindingsBindingBase base;
} NkBindingsBindingRelease;

typedef struct {
    guint64 binding;
    guint64 scope;
    NkBindingsBindingPress press;
    NkBindingsBindingRelease release;
} NkBindingsBinding;

typedef struct {
    NkBindingsBinding click;
    NkBindingsBinding dclick;
} NkBindingsBindingMouse;

static const gchar * const _nk_bindings_modifiers_names[] = {
    [NK_BINDINGS_MODIFIER_SHIFT   + NK_BINDINGS_NUM_MODIFIERS * 0] = "shift",
    [NK_BINDINGS_MODIFIER_CONTROL + NK_BINDINGS_NUM_MODIFIERS * 0] = "control",
    [NK_BINDINGS_MODIFIER_ALT     + NK_BINDINGS_NUM_MODIFIERS * 0] = "alt",
    [NK_BINDINGS_MODIFIER_SUPER   + NK_BINDINGS_NUM_MODIFIERS * 0] = "super",
    [NK_BINDINGS_MODIFIER_META    + NK_BINDINGS_NUM_MODIFIERS * 0] = "meta",
    [NK_BINDINGS_MODIFIER_HYPER   + NK_BINDINGS_NUM_MODIFIERS * 0] = "hyper",
    /* Allow a few aliases */
    [NK_BINDINGS_MODIFIER_SHIFT   + NK_BINDINGS_NUM_MODIFIERS * 1] = "shift_l",
    [NK_BINDINGS_MODIFIER_CONTROL + NK_BINDINGS_NUM_MODIFIERS * 1] = "control_l",
    [NK_BINDINGS_MODIFIER_ALT     + NK_BINDINGS_NUM_MODIFIERS * 1] = "alt_l",
    [NK_BINDINGS_MODIFIER_SUPER   + NK_BINDINGS_NUM_MODIFIERS * 1] = "super_l",
    [NK_BINDINGS_MODIFIER_META    + NK_BINDINGS_NUM_MODIFIERS * 1] = "meta_l",
    [NK_BINDINGS_MODIFIER_HYPER   + NK_BINDINGS_NUM_MODIFIERS * 1] = "hyper_l",
    [NK_BINDINGS_MODIFIER_SHIFT   + NK_BINDINGS_NUM_MODIFIERS * 2] = "shift_r",
    [NK_BINDINGS_MODIFIER_CONTROL + NK_BINDINGS_NUM_MODIFIERS * 2] = "control_r",
    [NK_BINDINGS_MODIFIER_ALT     + NK_BINDINGS_NUM_MODIFIERS * 2] = "alt_r",
    [NK_BINDINGS_MODIFIER_SUPER   + NK_BINDINGS_NUM_MODIFIERS * 2] = "super_r",
    [NK_BINDINGS_MODIFIER_META    + NK_BINDINGS_NUM_MODIFIERS * 2] = "meta_r",
    [NK_BINDINGS_MODIFIER_HYPER   + NK_BINDINGS_NUM_MODIFIERS * 2] = "hyper_r",
    [NK_BINDINGS_MODIFIER_SHIFT   + NK_BINDINGS_NUM_MODIFIERS * 3] = "",
    [NK_BINDINGS_MODIFIER_CONTROL + NK_BINDINGS_NUM_MODIFIERS * 3] = "ctrl",
    [NK_BINDINGS_MODIFIER_ALT     + NK_BINDINGS_NUM_MODIFIERS * 3] = "altgr",
    [NK_BINDINGS_MODIFIER_SUPER   + NK_BINDINGS_NUM_MODIFIERS * 3] = "logo",
    [NK_BINDINGS_MODIFIER_META    + NK_BINDINGS_NUM_MODIFIERS * 3] = "",
    [NK_BINDINGS_MODIFIER_HYPER   + NK_BINDINGS_NUM_MODIFIERS * 3] = "",
};

static const gchar * const _nk_bindings_mouse_button_names[] = {
    [NK_BINDINGS_MOUSE_BUTTON_PRIMARY] = "Primary",
    [NK_BINDINGS_MOUSE_BUTTON_SECONDARY] = "Secondary",
    [NK_BINDINGS_MOUSE_BUTTON_MIDDLE] = "Middle",
    [NK_BINDINGS_MOUSE_BUTTON_BACK] = "Back",
    [NK_BINDINGS_MOUSE_BUTTON_FORWARD] = "Forward",
};

static const gchar * const _nk_bindings_scroll_names[] = {
    [NK_SCROLL_TO_BINDING(0, NK_BINDINGS_SCROLL_AXIS_VERTICAL, -1)] = "Up",
    [NK_SCROLL_TO_BINDING(0, NK_BINDINGS_SCROLL_AXIS_VERTICAL,  1)] = "Down",
    [NK_SCROLL_TO_BINDING(0, NK_BINDINGS_SCROLL_AXIS_HORIZONTAL, -1)] = "Left",
    [NK_SCROLL_TO_BINDING(0, NK_BINDINGS_SCROLL_AXIS_HORIZONTAL,  1)] = "Right",
};

GQuark
nk_bindings_error(void)
{
    return g_quark_from_static_string("nk_bindings_error-quark");
}

static void
_nk_bindings_scope_free(gpointer data)
{
    NkBindingsScope *scope = data;

    g_hash_table_unref(scope->keycodes);
    g_hash_table_unref(scope->keysyms);
    g_hash_table_unref(scope->buttons);
    g_hash_table_unref(scope->scroll);

    g_slice_free(NkBindingsScope, scope);
}

NkBindings *
nk_bindings_new(guint64 double_click_delay)
{
    NkBindings *self;
    self = g_new0(NkBindings, 1);

    switch ( nk_xdg_de_detect() )
    {
    case NK_XDG_DE_NONE:
    case NK_XDG_DE_GNOME:
    case NK_XDG_DE_I3:
        /* Just use the GTK+ settings fallback */
    break;
    case NK_XDG_DE_KDE:
    {
        gchar *path;
        GKeyFile *settings;

        path = g_build_filename(g_get_user_config_dir(), "kcminputrc", NULL);
        settings = g_key_file_new();

        if ( g_key_file_load_from_file(settings, path, G_KEY_FILE_NONE, NULL) )
            self->double_click_delay = g_key_file_get_uint64(settings, "KDE", "DoubleClickInterval", NULL);
        g_key_file_free(settings);
        g_free(path);
    }
    break;
    }

    /* Always fallback to the GTK+ settings */
    const gchar *gtk_settings_keys[NK_GTK_SETTINGS_NUM_VERSION] = {
        "gtk-double-click-time",
        "gtk-double-click-time",
    };
    if ( self->double_click_delay < 1 )
        nk_gtk_settings_get_uint64(&self->double_click_delay, gtk_settings_keys);

    /* If nothing better, use the application value */
    if ( self->double_click_delay < 1 )
        self->double_click_delay = double_click_delay;

    /* If nothing better, use the application value */
    if ( self->double_click_delay < 1 )
        self->double_click_delay = NK_BINDINGS_DEFAULT_DOUBLE_CLICK_DELAY;

    return self;
}

static void _nk_bindings_seat_free(gpointer data);
void
nk_bindings_free(NkBindings *self)
{
    if ( self == NULL )
        return;

    g_list_free_full(self->seats, _nk_bindings_seat_free);

    nk_bindings_reset_bindings(self);

    g_free(self);
}

static void
_nk_bindings_binding_notify(NkBindingsBinding *binding)
{
    if ( binding->press.base.notify != NULL )
        binding->press.base.notify(binding->press.base.user_data);
    if ( binding->release.base.notify != NULL )
        binding->release.base.notify(binding->release.base.user_data);
}

static void
_nk_bindings_binding_free(gpointer data)
{
    NkBindingsBinding *binding = data;

    _nk_bindings_binding_notify(binding);

    g_slice_free(NkBindingsBinding, binding);
}

static void
_nk_bindings_binding_mouse_free(gpointer data)
{
    NkBindingsBindingMouse *binding = data;

    _nk_bindings_binding_notify(&binding->click);
    _nk_bindings_binding_notify(&binding->dclick);

    g_slice_free(NkBindingsBindingMouse, binding);
}

static gint
_nk_bindings_scope_compare(gconstpointer a, gconstpointer b)
{
    const NkBindingsScope *sa = a, *sb = b;
    return ( sb->id - sa->id );
}

static NkBindingsScope *
_nk_bindings_get_scope(NkBindings *self, guint64 scope_id)
{
    GList *link;
    NkBindingsScope *scope, cscope = { .id = scope_id };
    link = g_list_find_custom(self->scopes, &cscope, _nk_bindings_scope_compare);
    if ( link != NULL )
        scope = link->data;
    else
    {
        scope = g_slice_new(NkBindingsScope);
        scope->id = scope_id;
        scope->keycodes = g_hash_table_new_full(g_int64_hash, g_int64_equal, NULL, _nk_bindings_binding_free);
        scope->keysyms = g_hash_table_new_full(g_int64_hash, g_int64_equal, NULL, _nk_bindings_binding_free);
        scope->buttons = g_hash_table_new_full(g_int64_hash, g_int64_equal, NULL, _nk_bindings_binding_mouse_free);
        scope->scroll = g_hash_table_new_full(g_int64_hash, g_int64_equal, NULL, _nk_bindings_binding_free);
        self->scopes = g_list_insert_sorted(self->scopes, scope, _nk_bindings_scope_compare);
    }

    return scope;
}

static gboolean
_nk_bindings_parse_modifier(const gchar *string, xkb_mod_mask_t *mask)
{
    guint64 value;

    if ( ! nk_enum_parse(string, _nk_bindings_modifiers_names, G_N_ELEMENTS(_nk_bindings_modifiers_names), TRUE, FALSE, &value) )
        return FALSE;

    value %= NK_BINDINGS_NUM_MODIFIERS;

    *mask |= (1 << value);
    return TRUE;
}

gboolean
nk_bindings_add_binding(NkBindings *self, guint64 scope_id, const gchar *string, NkBindingsCallback callback, gpointer user_data, GDestroyNotify notify, GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    gboolean on_release = FALSE;
    xkb_mod_mask_t last_mask = 0;
    xkb_mod_mask_t mask = 0;
    xkb_keysym_t last_keysym = XKB_KEY_NoSymbol;
    xkb_keysym_t keysym = XKB_KEY_NoSymbol;

    const gchar *w = string;

    enum {
        NK_BINDINGS_MODE_NONE,
        NK_BINDINGS_MODE_EXCLAMATION  = '!',
        NK_BINDINGS_MODE_GTK   = '>',
        NK_BINDINGS_MODE_PLUS  = '+',
        NK_BINDINGS_MODE_MINUS = '-',
    } mode = NK_BINDINGS_MODE_NONE;

    if ( g_utf8_get_char(w) == '!' )
    {
        w = g_utf8_next_char(w);
        on_release = TRUE;
        mode = NK_BINDINGS_MODE_EXCLAMATION;
    }

    gsize l;
    gchar *tmp;
    const gchar *e, *s;
    l = strlen(w);
    tmp = g_newa(gchar, l + 1);
    e = w + l;
    s = w;
    for ( e = w + l ; w < e ; w = g_utf8_next_char(w) )
    {
        gunichar wc = g_utf8_get_char(w);
        switch ( wc )
        {
        case '!':
            g_set_error(error, NK_BINDINGS_ERROR, NK_BINDINGS_ERROR_SYNTAX, "Syntax error in binding '%s': '!' must be the first character", string);
            return FALSE;
        case '<':
            if ( ( mode != NK_BINDINGS_MODE_NONE ) && ( mode != NK_BINDINGS_MODE_GTK ) )
            {
                g_set_error(error, NK_BINDINGS_ERROR, NK_BINDINGS_ERROR_SYNTAX, "Syntax error in binding '%s': you cannot mix syntaxes", string);
                return FALSE;
            }
            mode = NK_BINDINGS_MODE_GTK;
            s = g_utf8_next_char(w);
            continue;
        case '>':
        case '+':
        case '-':
            if ( mode == NK_BINDINGS_MODE_NONE )
            {
                if ( wc == '>' )
                {
                    g_set_error(error, NK_BINDINGS_ERROR, NK_BINDINGS_ERROR_SYNTAX, "Syntax error in binding '%s': expected identifier or '<', got '>'", string);
                    return FALSE;
                }
                mode = wc;
            }
            else if ( mode == NK_BINDINGS_MODE_EXCLAMATION )
            {
                if ( wc != '>' )
                    mode = wc;
            }
            if ( mode != wc )
            {
                g_set_error(error, NK_BINDINGS_ERROR, NK_BINDINGS_ERROR_SYNTAX, "Syntax error in binding '%s': you cannot mix syntaxes", string);
                return FALSE;
            }
            if ( s == w )
            {
                g_set_error(error, NK_BINDINGS_ERROR, NK_BINDINGS_ERROR_SYNTAX, "Syntax error in binding '%s': cannot have an empty modifier", string);
                return FALSE;
            }

            g_snprintf(tmp, w - s + 1, "%s", s);
            if ( mode != NK_BINDINGS_MODE_GTK )
            {
                last_mask = mask;
                last_keysym = xkb_keysym_from_name(tmp, XKB_KEYSYM_NO_FLAGS);
            }
            if ( ! _nk_bindings_parse_modifier(tmp, &mask) )
            {
                if ( g_ascii_strcasecmp(tmp, "release") == 0 )
                {
                    if ( on_release )
                    {
                        g_set_error(error, NK_BINDINGS_ERROR, NK_BINDINGS_ERROR_SYNTAX, "Syntax error in binding '%s': cannot have on-release syntax twice", string);
                        return FALSE;
                    }
                    on_release = TRUE;
                }
                else
                {
                    g_set_error(error, NK_BINDINGS_ERROR, NK_BINDINGS_ERROR_SYNTAX, "Syntax error in binding '%s': cannot have two non-modifiers", string);
                    return FALSE;
                }
            }
            s = g_utf8_next_char(w);
        break;
        case '[':
            w = g_utf8_prev_char(e);
        break;
        default:
            if ( g_unichar_isalnum(wc) || ( wc == '_' ) )
                break;
            g_set_error(error, NK_BINDINGS_ERROR, NK_BINDINGS_ERROR_SYNTAX, "Syntax error in binding '%s': unexpected character '%.*s'", string, (gint) ( g_utf8_next_char(w) - w ),  w);
            return FALSE;
        }
    }

    NkBindingsScope *scope;
    scope = _nk_bindings_get_scope(self, scope_id);

    NkBindingsBinding *binding = NULL;
    if ( s < e )
    {
        if ( g_utf8_get_char(s) == '[' )
        {
            s = g_utf8_next_char(s);

            guint64 keycode;
            gchar *ce;
            errno = 0;
            keycode = g_ascii_strtoull(s, &ce, 10);
            if ( s == ce )
            {
                g_set_error(error, NK_BINDINGS_ERROR, NK_BINDINGS_ERROR_SYNTAX, "Syntax error in binding '%s': could not parse keycode", string);
                return FALSE;
            }
            else if ( ( g_utf8_get_char(ce) != ']' ) || ( e != g_utf8_next_char(ce) ) )
            {
                g_set_error(error, NK_BINDINGS_ERROR, NK_BINDINGS_ERROR_SYNTAX, "Syntax error in binding '%s': keycode must be the end of the binding string, enclosed in squared brackets '[]'", string);
                return FALSE;
            }
            else if ( ! xkb_keycode_is_legal_ext(keycode) )
            {
                g_set_error(error, NK_BINDINGS_ERROR, NK_BINDINGS_ERROR_SYNTAX, "Syntax error in binding '%s': wrong keycode value %"G_GINT64_MODIFIER"u", string, keycode);
                return FALSE;
            }

            guint64 value = NK_KEYCODE_TO_BINDING(mask, keycode);
            binding = g_hash_table_lookup(scope->keycodes, &value);
            if ( binding == NULL )
            {
                binding = g_slice_new0(NkBindingsBinding);
                binding->binding = value;
                g_hash_table_insert(scope->keycodes, &binding->binding, binding);
            }
        }
        else if ( g_ascii_strncasecmp(s, "Mouse", strlen("Mouse")) == 0 )
        {
            s += strlen("Mouse");

            gboolean double_click = FALSE;
            if ( g_unichar_toupper(g_utf8_get_char(s)) == 'D' )
            {
                s = g_utf8_next_char(s);
                double_click = TRUE;
            }

            guint64 button;
            if ( g_ascii_strncasecmp(s, "Extra", strlen("Extra")) == 0 )
            {
                s += strlen("Extra");
                gchar *ce;
                errno = 0;
                button = g_ascii_strtoull(s, &ce, 10);
                if ( s == ce )
                {
                    g_set_error(error, NK_BINDINGS_ERROR, NK_BINDINGS_ERROR_SYNTAX, "Syntax error in binding '%s': could not parse mouse extra button number", string);
                    return FALSE;
                }
                else if ( e != ce )
                {
                    g_set_error(error, NK_BINDINGS_ERROR, NK_BINDINGS_ERROR_SYNTAX, "Syntax error in binding '%s': mouse extra button must be the end of the binding string", string);
                    return FALSE;
                }
                button += NK_BINDINGS_MOUSE_BUTTON_EXTRA;
            }
            else if ( ! nk_enum_parse(s, _nk_bindings_mouse_button_names, G_N_ELEMENTS(_nk_bindings_mouse_button_names), TRUE, FALSE, &button) )
            {
                g_set_error(error, NK_BINDINGS_ERROR, NK_BINDINGS_ERROR_SYNTAX, "Syntax error in binding '%s': unknown mouse button %s", string, s);
                return FALSE;
            }

            guint64 value = NK_BUTTON_TO_BINDING(mask, button);
            NkBindingsBindingMouse *mouse_binding;
            mouse_binding = g_hash_table_lookup(scope->buttons, &value);
            if ( mouse_binding == NULL )
            {
                mouse_binding = g_slice_new0(NkBindingsBindingMouse);
                mouse_binding->click.binding = value;
                mouse_binding->dclick.binding = value;
                g_hash_table_insert(scope->buttons, &mouse_binding->click.binding, mouse_binding);
            }

            if ( double_click )
                binding = &mouse_binding->dclick;
            else
                binding = &mouse_binding->click;
        }
        else if ( g_ascii_strncasecmp(s, "Scroll", strlen("Scroll")) == 0 )
        {
            s += strlen("Scroll");

            if ( on_release )
            {
                g_set_error(error, NK_BINDINGS_ERROR, NK_BINDINGS_ERROR_SYNTAX, "Syntax error in binding '%s': scroll action cannot be bound on release", string);
                return FALSE;
            }

            guint64 scroll;
            if ( ! nk_enum_parse(s, _nk_bindings_scroll_names, G_N_ELEMENTS(_nk_bindings_scroll_names), TRUE, FALSE, &scroll) )
            {
                g_set_error(error, NK_BINDINGS_ERROR, NK_BINDINGS_ERROR_SYNTAX, "Syntax error in binding '%s': unknown scroll direction %s", string, s);
                return FALSE;
            }

            guint64 value = _NK_VALUE_TO_BINDING(mask, scroll);
            binding = g_hash_table_lookup(scope->scroll, &value);
            if ( binding == NULL )
            {
                binding = g_slice_new0(NkBindingsBinding);
                binding->binding = value;
                g_hash_table_insert(scope->scroll, &binding->binding, binding);
            }
        }
        else
            keysym = xkb_keysym_from_name(s, XKB_KEYSYM_NO_FLAGS);
    }

    if ( binding == NULL )
    {
        if ( keysym == XKB_KEY_NoSymbol )
        {
            if  ( last_keysym == XKB_KEY_NoSymbol )
            {
                g_set_error(error, NK_BINDINGS_ERROR, NK_BINDINGS_ERROR_ALREADY_REGISTERED, "No keysym found for binding '%s'", string);
                return FALSE;
            }
            keysym = last_keysym;
            mask = last_mask;
        }
        guint64 value = NK_KEYSYM_TO_BINDING(mask, keysym);
        binding = g_hash_table_lookup(scope->keysyms, &value);
        if ( binding == NULL )
        {
            binding = g_slice_new0(NkBindingsBinding);
            binding->binding = value;
            g_hash_table_insert(scope->keysyms, &binding->binding, binding);
        }
    }

    NkBindingsBindingBase *base = on_release ? &binding->release.base : &binding->press.base;

    if ( base->callback != NULL )
    {
        g_set_error(error, NK_BINDINGS_ERROR, NK_BINDINGS_ERROR_ALREADY_REGISTERED, "There is already a binding matching '%s'", string);
        return FALSE;
    }

    binding->scope = scope_id;
    base->callback = callback;
    base->user_data = user_data;
    base->notify = notify;

    return TRUE;
}

void
nk_bindings_reset_bindings(NkBindings *self)
{
    g_list_free_full(self->scopes, _nk_bindings_scope_free);
    self->scopes = NULL;
}

static gboolean
_nk_bindings_seat_binding_trigger(NkBindingsSeat *self, NkBindingsBinding *binding, gpointer target, gboolean trigger)
{
    if ( binding == NULL )
        return FALSE;

    gboolean handled = FALSE;
    gboolean has_press = ( binding->press.base.callback != NULL );
    gboolean on_release_waiting = ( g_list_find(self->on_release, binding) != NULL );
    if ( trigger && has_press )
        handled = binding->press.base.callback(binding->scope, target, binding->press.base.user_data);
    if ( ( ! on_release_waiting ) && ( binding->release.base.callback != NULL ) && ( handled || ( ! trigger ) || ( ! has_press ) ) )
    {
        self->on_release = g_list_prepend(self->on_release, binding);
        on_release_waiting = TRUE;
    }

    return ( handled || on_release_waiting );
}

static gboolean
_nk_bindings_try_key_bindings(NkBindings *self, NkBindingsSeat *seat, gpointer target, guint64 keycode, guint64 keysym, gboolean trigger)
{
    GList *scope_;
    for ( scope_ = self->scopes ; scope_ != NULL ; scope_ = g_list_next(scope_) )
    {
        NkBindingsScope *scope = scope_->data;
        if ( _nk_bindings_seat_binding_trigger(seat, g_hash_table_lookup(scope->keycodes, &keycode), target, trigger) )
            return TRUE;
        if ( ( keysym != XKB_KEY_NoSymbol ) && _nk_bindings_seat_binding_trigger(seat, g_hash_table_lookup(scope->keysyms, &keysym), target, trigger) )
            return TRUE;
    }

    return FALSE;
}

static gboolean
_nk_bindings_try_button_bindings(NkBindings *self, NkBindingsSeat *seat, gpointer target, guint64 **button, guint64 time)
{
    NkBindingsBindingMouse *mouse_binding = NULL;
    GList *scope_;
    for ( scope_ = self->scopes ; scope_ != NULL ; scope_ = g_list_next(scope_) )
    {
        NkBindingsScope *scope = scope_->data;

        mouse_binding = g_hash_table_lookup(scope->buttons, *button);
        if ( mouse_binding == NULL )
            continue;

        if ( time < self->double_click_delay )
        {
            if ( _nk_bindings_seat_binding_trigger(seat, &mouse_binding->dclick, target, TRUE) )
            {
                *button = &mouse_binding->click.binding;
                return TRUE;
            }
        }
        if ( _nk_bindings_seat_binding_trigger(seat, &mouse_binding->click, target, TRUE) )
        {
            *button = &mouse_binding->click.binding;
            return TRUE;
        }
    }
    return FALSE;
}

static NkBindingsBinding *
_nk_bindings_try_scroll_bindings(NkBindings *self, NkBindingsSeat *seat, gpointer target, guint64 scroll)
{
    GList *scope_;
    for ( scope_ = self->scopes ; scope_ != NULL ; scope_ = g_list_next(scope_) )
    {
        NkBindingsScope *scope = scope_->data;
        NkBindingsBinding *binding;

        binding = g_hash_table_lookup(scope->scroll, &scroll);
        if ( _nk_bindings_seat_binding_trigger(seat, binding, target, TRUE) )
            return binding;
    }

    return NULL;
}

NkBindingsSeat *
nk_bindings_seat_new(NkBindings *bindings, enum xkb_context_flags flags)
{
    struct xkb_context *context;
    NkBindingsSeat *self;

    context = xkb_context_new(flags);
    if ( context == NULL )
        return NULL;

    self = g_new0(NkBindingsSeat, 1);
    self->bindings = bindings;
    self->context = context;

#ifdef NK_XKBCOMMON_HAS_COMPOSE
    self->compose.table = xkb_compose_table_new_from_locale(self->context, setlocale(LC_CTYPE, NULL), 0);
    if ( self->compose.table != NULL )
        self->compose.state = xkb_compose_state_new(self->compose.table, 0);
#endif /* NK_XKBCOMMON_HAS_COMPOSE */

    self->last_timestamps = g_hash_table_new_full(g_int64_hash, g_int64_equal, NULL, g_free);
    return self;
}

static void _nk_bindings_seat_free_on_release(NkBindingsSeat *self, gpointer target, gboolean trigger);
static void
_nk_bindings_seat_free(gpointer data)
{
    NkBindingsSeat *self = data;

    _nk_bindings_seat_free_on_release(self, NULL, FALSE);

    xkb_keymap_unref(self->keymap);
    xkb_state_unref(self->state);

    xkb_context_unref(self->context);

    g_free(self);
}

void
nk_bindings_seat_free(NkBindingsSeat *self)
{
    if ( self == NULL )
        return;

    self->bindings->seats = g_list_delete_link(self->bindings->seats, self->link);
    _nk_bindings_seat_free(self);
}

static void
_nk_bindings_seat_find_modifier(NkBindingsSeat *self, NkBindingsModifiers modifier, ...)
{
    va_list names;
    const gchar *name;
    xkb_mod_index_t i, *m = self->modifiers[modifier];
    va_start(names, modifier);
    while ( ( name = va_arg(names, const gchar *) ) != NULL )
    {
        i = xkb_keymap_mod_get_index(self->keymap, name);
        if ( i != XKB_MOD_INVALID )
            *m++ = i;
    }
    *m = XKB_MOD_INVALID;
    va_end(names);
}

void
nk_bindings_seat_update_keymap(NkBindingsSeat *self, struct xkb_keymap *keymap, struct xkb_state *state)
{
    g_return_if_fail(self != NULL);
    g_return_if_fail((keymap != NULL && state != NULL) || (keymap == NULL && state == NULL));

    xkb_keymap_unref(self->keymap);
    xkb_state_unref(self->state);

    if ( keymap == NULL )
    {
        nk_bindings_seat_reset(self);
        self->keymap = NULL;
        self->state = NULL;
        gsize i;
        for ( i = 0 ; i < NK_BINDINGS_NUM_MODIFIERS ; ++i )
            self->modifiers[i][0] = XKB_MOD_INVALID;
        return;
    }

    self->keymap = xkb_keymap_ref(keymap);
    self->state = xkb_state_ref(state);

    _nk_bindings_seat_find_modifier(self, NK_BINDINGS_MODIFIER_SHIFT, XKB_MOD_NAME_SHIFT, NULL);
    _nk_bindings_seat_find_modifier(self, NK_BINDINGS_MODIFIER_CONTROL, XKB_MOD_NAME_CTRL, NULL);
    _nk_bindings_seat_find_modifier(self, NK_BINDINGS_MODIFIER_ALT, XKB_MOD_NAME_ALT, "Alt", "LAlt", "RAlt", NULL);
    _nk_bindings_seat_find_modifier(self, NK_BINDINGS_MODIFIER_META, "Meta", NULL);
    _nk_bindings_seat_find_modifier(self, NK_BINDINGS_MODIFIER_SUPER, XKB_MOD_NAME_LOGO, "Super", NULL);
    _nk_bindings_seat_find_modifier(self, NK_BINDINGS_MODIFIER_HYPER, "Hyper", NULL);
}

struct xkb_context *
nk_bindings_seat_get_context(NkBindingsSeat *self)
{
    return self->context;
}


static void
_nk_bindings_seat_get_modifiers_masks(NkBindingsSeat *self, xkb_keycode_t key, xkb_mod_mask_t *effective, xkb_mod_mask_t *not_consumed)
{
    *effective = 0;
    *not_consumed = 0;

    if ( self->state == NULL )
        return;

    NkBindingsModifiers mod;
    xkb_mod_index_t *i;
    for ( mod = 0 ; mod < NK_BINDINGS_NUM_MODIFIERS ; ++mod )
    {
        for ( i = self->modifiers[mod] ; *i != XKB_MOD_INVALID ; ++i )
        {
            if ( ! xkb_state_mod_index_is_active(self->state, *i, XKB_STATE_MODS_EFFECTIVE) )
                continue;
            *effective |= (1 << mod);
#ifdef NK_XKBCOMMON_HAS_CONSUMED2
            if ( xkb_state_mod_index_is_consumed2(self->state, key, *i, XKB_CONSUMED_MODE_GTK) != 1 )
#else /* ! NK_XKBCOMMON_HAS_COMSUMED2 */
            if ( xkb_state_mod_index_is_consumed(self->state, key, *i) != 1 )
#endif /* ! NK_XKBCOMMON_HAS_COMSUMED2 */
                *not_consumed |= (1 << mod);

            break;
        }
    }
}

static void
_nk_bindings_seat_free_on_release(NkBindingsSeat *self, gpointer target, gboolean trigger)
{
    if ( self->on_release == NULL )
        return;

    GList *link, *next;
    for ( link = self->on_release, next = g_list_next(link) ; link != NULL ; link = next, next = g_list_next(link) )
    {
        NkBindingsBinding *binding = link->data;
        if ( trigger )
            binding->release.base.callback(binding->scope, target, binding->release.base.user_data);
        g_list_free_1(link);
    }
    self->on_release = NULL;
}

gchar *
nk_bindings_seat_handle_key(NkBindingsSeat *self, gpointer target, xkb_keycode_t keycode, NkBindingsKeyState state)
{
    g_return_val_if_fail(self != NULL, NULL);
    g_return_val_if_fail(self->keymap != NULL, NULL);
    g_return_val_if_fail(self->state != NULL, NULL);

    xkb_keysym_t keysym;
    gchar *tmp = NULL;
    gsize length = 0;

    if ( state == NK_BINDINGS_KEY_STATE_RELEASE )
    {
        xkb_mod_mask_t dummy, mask;
        _nk_bindings_seat_get_modifiers_masks(self, 0, &dummy, &mask);
        if ( mask == 0 )
            _nk_bindings_seat_free_on_release(self, target, TRUE);
        return NULL;
    }

    keysym = xkb_state_key_get_one_sym(self->state, keycode);

    gboolean regular_press = ( state == NK_BINDINGS_KEY_STATE_PRESS );

#ifdef NK_XKBCOMMON_HAS_COMPOSE
    if ( regular_press && ( self->compose.state != NULL ) && ( keysym != XKB_KEY_NoSymbol ) && ( xkb_compose_state_feed(self->compose.state, keysym) == XKB_COMPOSE_FEED_ACCEPTED ) )
    {
        switch ( xkb_compose_state_get_status(self->compose.state) )
        {
        case XKB_COMPOSE_CANCELLED:
            /* Eat the keysym that cancelled the compose sequence.
             * This is default behaviour with Xlib */
        case XKB_COMPOSE_COMPOSING:
            return NULL;
        case XKB_COMPOSE_COMPOSED:
            length = xkb_compose_state_get_utf8(self->compose.state, NULL, 0);
            if ( length != 0 )
            {
                tmp = g_new(gchar, length + 1);
                length = xkb_compose_state_get_utf8(self->compose.state, tmp, length + 1);
            }
            return tmp;
        case XKB_COMPOSE_NOTHING:
        break;
        }
    }
#endif /* NK_XKBCOMMON_HAS_COMPOSE */

    xkb_mod_mask_t effective, not_consumed;
    _nk_bindings_seat_get_modifiers_masks(self, keycode, &effective, &not_consumed);

    if ( _nk_bindings_try_key_bindings(self->bindings, self, target, NK_KEYCODE_TO_BINDING(effective, keycode), NK_KEYSYM_TO_BINDING(not_consumed, keysym), regular_press) )
        return NULL;

    if ( ! regular_press )
        return NULL;

    if ( length == 0 )
    {
        length = xkb_state_key_get_utf8(self->state, keycode, NULL, 0);
        if ( length != 0 )
        {
            tmp = g_new(gchar, length + 1);
            length = xkb_state_key_get_utf8(self->state, keycode, tmp, length + 1);
        }
    }

    return tmp;
}

gchar *
nk_bindings_seat_handle_key_with_modmask(NkBindingsSeat *self, gpointer target, xkb_mod_mask_t modmask, xkb_keycode_t keycode, NkBindingsKeyState state)
{
    g_return_val_if_fail(self != NULL, NULL);
    g_return_val_if_fail(self->keymap != NULL, NULL);
    g_return_val_if_fail(self->state != NULL, NULL);

    xkb_mod_mask_t effective_mods, depressed_mods, latched_mods, locked_mods;
    xkb_layout_index_t depressed_layout, latched_layout, locked_layout;
    gchar *ret;

    effective_mods = xkb_state_serialize_mods(self->state, XKB_STATE_MODS_EFFECTIVE);
    depressed_mods = xkb_state_serialize_mods(self->state, XKB_STATE_MODS_DEPRESSED);
    latched_mods = xkb_state_serialize_mods(self->state, XKB_STATE_MODS_LATCHED);
    locked_mods = xkb_state_serialize_mods(self->state, XKB_STATE_MODS_LOCKED);
    depressed_layout = xkb_state_serialize_layout(self->state, XKB_STATE_LAYOUT_DEPRESSED);
    latched_layout = xkb_state_serialize_layout(self->state, XKB_STATE_LAYOUT_LATCHED);
    locked_layout = xkb_state_serialize_layout(self->state, XKB_STATE_LAYOUT_LOCKED);

    if ( effective_mods == modmask )
        return nk_bindings_seat_handle_key(self, target, keycode, state);

    xkb_state_update_mask(self->state, modmask, 0, 0, depressed_layout, latched_layout, locked_layout);
    ret = nk_bindings_seat_handle_key(self, target, keycode, state);
    xkb_state_update_mask(self->state, depressed_mods, latched_mods, locked_mods, depressed_layout, latched_layout, locked_layout);

    return ret;
}

gboolean
nk_bindings_seat_handle_button(NkBindingsSeat *self, gpointer target, NkBindingsMouseButton button, NkBindingsButtonState state, guint64 timestamp)
{
    g_return_val_if_fail(self != NULL, FALSE);

    xkb_mod_mask_t dummy, mask;

    _nk_bindings_seat_get_modifiers_masks(self, 0, &dummy, &mask);

    if ( state == NK_BINDINGS_BUTTON_STATE_RELEASE )
    {
        if ( mask == 0 )
            _nk_bindings_seat_free_on_release(self, target, TRUE);
        return TRUE;
    }

    guint64 binding = NK_BUTTON_TO_BINDING(mask, button), *binding_ = &binding;
    guint64 *last_timestamp, time = timestamp;

    last_timestamp = g_hash_table_lookup(self->last_timestamps, &binding);
    if ( last_timestamp != NULL )
        time -= *last_timestamp;
    if ( ! _nk_bindings_try_button_bindings(self->bindings, self, target, &binding_, time) )
        return FALSE;
    if ( last_timestamp == NULL )
        g_hash_table_insert(self->last_timestamps, binding_, g_memdup(&timestamp, sizeof(guint64)));
    else
        *last_timestamp = timestamp;
    return TRUE;
}

gboolean
nk_bindings_seat_handle_scroll(NkBindingsSeat *self, gpointer target, NkBindingsScrollAxis axis, gint32 step)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(step != 0, FALSE);

    xkb_mod_mask_t dummy, mask;

    _nk_bindings_seat_get_modifiers_masks(self, 0, &dummy, &mask);

    guint64 binding_ = NK_SCROLL_TO_BINDING(mask, axis, step);

    NkBindingsBinding *binding;
    binding = _nk_bindings_try_scroll_bindings(self->bindings, self, target, binding_);

    if ( binding == NULL )
        return FALSE;

    for ( step = ABS(step) ; step > 1 ; --step )
        _nk_bindings_seat_binding_trigger(self, binding, target, TRUE);
    return TRUE;
}

void
nk_bindings_seat_update_mask(NkBindingsSeat *self, gpointer target, xkb_mod_mask_t depressed_mods, xkb_mod_mask_t latched_mods, xkb_mod_mask_t locked_mods, xkb_layout_index_t depressed_layout, xkb_layout_index_t latched_layout, xkb_layout_index_t locked_layout)
{
    g_return_if_fail(self != NULL);
    g_return_if_fail(self->state != NULL);
    g_return_if_fail(self->keymap != NULL);

    enum xkb_state_component changed;

    changed = xkb_state_update_mask(self->state, depressed_mods, latched_mods, locked_mods, depressed_layout, latched_layout, locked_layout);

    if ( changed & XKB_STATE_MODS_EFFECTIVE )
    {
        xkb_mod_mask_t dummy, mask;
        _nk_bindings_seat_get_modifiers_masks(self, 0, &dummy, &mask);
        if ( mask == 0 )
            _nk_bindings_seat_free_on_release(self, target, TRUE);
    }
}

void
nk_bindings_seat_reset(NkBindingsSeat *self)
{
    g_return_if_fail(self != NULL);

    _nk_bindings_seat_free_on_release(self, NULL, FALSE);
}
