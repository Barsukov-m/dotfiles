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

#include <locale.h>

#include <glib.h>

#include <nkutils-bindings.h>

#define MAX_BINDINGS 20
#define MAX_KEY_SEQUENCE 20

typedef struct {
    NkBindings *bindings;
    struct xkb_keymap *keymap;
    struct xkb_state *master_state;
    struct xkb_state *state;
    NkBindingsSeat *seat;
    guint64 triggered;
} NkBindingsTestFixture;

typedef struct {
    const gchar *binding;
    guint64 trigger;
} NkBindingsTestBinding;

typedef struct {
    NkBindingsTestFixture *fixture;
    guint64 trigger;
} NkBindingsTestBindingSlice;

typedef struct {
    xkb_keycode_t key;
    NkBindingsKeyState state;
    gboolean fail;
    const gchar *text;
    guint64 triggered;
} NkBindingsTestKey;

typedef struct {
    NkBindingsTestBinding bindings[MAX_BINDINGS + 1];
    NkBindingsTestKey keys[MAX_KEY_SEQUENCE + 1];
} NkBindingsTestData;

/* Using US basic keymap */
#define KEYCODE_NONE      0
#define KEYCODE_ESCAPE    9
#define KEYCODE_Q         24
#define KEYCODE_CONTROL_L 37
#define KEYCODE_A         38
#define KEYCODE_S         39
#define KEYCODE_D         40
#define KEYCODE_TILDE     49
#define KEYCODE_SHIFT_L   50
#define KEYCODE_ALT_L     64
#define KEYCODE_SPACE     65
#define KEYCODE_ALT_GR    108
#define KEYCODE_SUPER_L   133

#define _NK_BINDINGS_KEYCODE_TO_STRING(s) #s
#define NK_BINDINGS_KEYCODE_TO_STRING(s) _NK_BINDINGS_KEYCODE_TO_STRING(s)

static const gchar *_nk_bindings_test_key_names[] = {
    [KEYCODE_ESCAPE] = "Escape",
    [KEYCODE_Q] = "Q",
    [KEYCODE_CONTROL_L] = "Control_L",
    [KEYCODE_A] = "A",
    [KEYCODE_S] = "S",
    [KEYCODE_D] = "D",
    [KEYCODE_TILDE] = "`",
    [KEYCODE_SHIFT_L] = "Shift_L",
    [KEYCODE_ALT_L] = "Alt_L",
    [KEYCODE_SPACE] = "Space",
    [KEYCODE_ALT_GR] = "AltGr/ISO_Level3_Shift",
    [KEYCODE_SUPER_L] = "Super_L",
};

static const struct {
    const gchar *testpath;
    NkBindingsTestData data;
} _nk_bindings_tests_list[] = {
    {
        .testpath = "/nkutils/bindings/press/key/found",
        .data = {
            .bindings = {
                { .binding = "s", .trigger = 1 },
                { .binding = NULL }
            },
            .keys = {
                { .key = KEYCODE_A, .text = "a" },
                { .key = KEYCODE_A, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_S, .triggered = 1 },
                { .key = KEYCODE_S, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_D, .text = "d" },
                { .key = KEYCODE_D, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_NONE }
            },
        }
    },
    {
        .testpath = "/nkutils/bindings/press/key/not-found/modified",
        .data = {
            .bindings = {
                { .binding = "s", .trigger = 1 },
                { .binding = NULL }
            },
            .keys = {
                { .key = KEYCODE_A, .text = "a" },
                { .key = KEYCODE_A, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_CONTROL_L },
                { .key = KEYCODE_S, .text = "\x13" },
                { .key = KEYCODE_S, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_CONTROL_L, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_D, .text = "d" },
                { .key = KEYCODE_D, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_NONE }
            },
        }
    },
    {
        .testpath = "/nkutils/bindings/press/combo/minus",
        .data = {
            .bindings = {
                { .binding = "Ctrl-s", .trigger = 1 },
                { .binding = NULL }
            },
            .keys = {
                { .key = KEYCODE_A, .text = "a" },
                { .key = KEYCODE_A, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_CONTROL_L },
                { .key = KEYCODE_S, .triggered = 1 },
                { .key = KEYCODE_S, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_CONTROL_L, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_D, .text = "d" },
                { .key = KEYCODE_D, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_NONE }
            },
        }
    },
    {
        .testpath = "/nkutils/bindings/press/combo/plus",
        .data = {
            .bindings = {
                { .binding = "Ctrl+s", .trigger = 1 },
                { .binding = NULL }
            },
            .keys = {
                { .key = KEYCODE_A, .text = "a" },
                { .key = KEYCODE_A, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_CONTROL_L },
                { .key = KEYCODE_S, .triggered = 1 },
                { .key = KEYCODE_S, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_CONTROL_L, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_D, .text = "d" },
                { .key = KEYCODE_D, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_NONE }
            },
        }
    },
    {
        .testpath = "/nkutils/bindings/press/combo/gtk",
        .data = {
            .bindings = {
                { .binding = "<Ctrl>s", .trigger = 1 },
                { .binding = NULL }
            },
            .keys = {
                { .key = KEYCODE_A, .text = "a" },
                { .key = KEYCODE_A, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_CONTROL_L },
                { .key = KEYCODE_S, .triggered = 1 },
                { .key = KEYCODE_S, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_CONTROL_L, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_D, .text = "d" },
                { .key = KEYCODE_D, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_NONE }
            },
        }
    },
    {
        .testpath = "/nkutils/bindings/press/combo/multi-modifier",
        .data = {
            .bindings = {
                { .binding = "<Ctrl><Alt><Super>S", .trigger = 1 },
                { .binding = NULL }
            },
            .keys = {
                { .key = KEYCODE_A, .text = "a" },
                { .key = KEYCODE_A, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_CONTROL_L },
                { .key = KEYCODE_ALT_L },
                { .key = KEYCODE_SUPER_L },
                { .key = KEYCODE_SHIFT_L },
                { .key = KEYCODE_S, .triggered = 1 },
                { .key = KEYCODE_S, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_CONTROL_L, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_SHIFT_L, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_SUPER_L, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_ALT_L, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_D, .text = "d" },
                { .key = KEYCODE_D, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_NONE }
            },
        }
    },
    {
        .testpath = "/nkutils/bindings/press/consume/consumed",
        .data = {
            .bindings = {
                { .binding = "S", .trigger = 1 },
                { .binding = NULL }
            },
            .keys = {
                { .key = KEYCODE_A, .text = "a" },
                { .key = KEYCODE_A, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_SHIFT_L },
                { .key = KEYCODE_S, .triggered = 1 },
                { .key = KEYCODE_S, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_SHIFT_L, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_D, .text = "d" },
                { .key = KEYCODE_D, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_NONE }
            },
        }
    },
    {
        .testpath = "/nkutils/bindings/press/consume/not-consumed",
        .data = {
            .bindings = {
                { .binding = "s", .trigger = 1 },
                { .binding = NULL }
            },
            .keys = {
                { .key = KEYCODE_A, .text = "a" },
                { .key = KEYCODE_A, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_SHIFT_L },
                { .key = KEYCODE_S, .text = "S" },
                { .key = KEYCODE_S, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_SHIFT_L, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_D, .text = "d" },
                { .key = KEYCODE_D, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_NONE }
            },
        }
    },
    {
        .testpath = "/nkutils/bindings/press/keycode/not-consumed",
        .data = {
            .bindings = {
                { .binding = "<Shift>[" NK_BINDINGS_KEYCODE_TO_STRING(KEYCODE_S) "]", .trigger = 1 },
                { .binding = NULL }
            },
            .keys = {
                { .key = KEYCODE_A, .text = "a" },
                { .key = KEYCODE_A, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_SHIFT_L },
                { .key = KEYCODE_S, .triggered = 1 },
                { .key = KEYCODE_S, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_SHIFT_L, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_D, .text = "d" },
                { .key = KEYCODE_D, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_NONE }
            },
        }
    },
    {
        .testpath = "/nkutils/bindings/press/keycode/consumed",
        .data = {
            .bindings = {
                { .binding = "[" NK_BINDINGS_KEYCODE_TO_STRING(KEYCODE_S) "]", .trigger = 1 },
                { .binding = NULL }
            },
            .keys = {
                { .key = KEYCODE_A, .text = "a" },
                { .key = KEYCODE_A, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_SHIFT_L },
                { .key = KEYCODE_S, .text = "S" },
                { .key = KEYCODE_S, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_SHIFT_L, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_D, .text = "d" },
                { .key = KEYCODE_D, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_NONE }
            },
        }
    },
    {
        .testpath = "/nkutils/bindings/release/key/exclamation",
        .data = {
            .bindings = {
                { .binding = "!s", .trigger = 1 },
                { .binding = NULL }
            },
            .keys = {
                { .key = KEYCODE_A, .text = "a" },
                { .key = KEYCODE_A, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_S },
                { .key = KEYCODE_S, .state = NK_BINDINGS_KEY_STATE_RELEASE, .triggered = 1 },
                { .key = KEYCODE_D, .text = "d" },
                { .key = KEYCODE_D, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_NONE }
            },
        }
    },
    {
        .testpath = "/nkutils/bindings/release/key/gtk",
        .data = {
            .bindings = {
                { .binding = "<Release>s", .trigger = 1 },
                { .binding = NULL }
            },
            .keys = {
                { .key = KEYCODE_A, .text = "a" },
                { .key = KEYCODE_A, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_S },
                { .key = KEYCODE_S, .state = NK_BINDINGS_KEY_STATE_RELEASE, .triggered = 1 },
                { .key = KEYCODE_D, .text = "d" },
                { .key = KEYCODE_D, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_NONE }
            },
        }
    },
    {
        .testpath = "/nkutils/bindings/release/combo/simple",
        .data = {
            .bindings = {
                { .binding = "<Release><Ctrl>s", .trigger = 1 },
                { .binding = NULL }
            },
            .keys = {
                { .key = KEYCODE_A, .text = "a" },
                { .key = KEYCODE_A, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_CONTROL_L },
                { .key = KEYCODE_S },
                { .key = KEYCODE_S, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_CONTROL_L, .state = NK_BINDINGS_KEY_STATE_RELEASE, .triggered = 1 },
                { .key = KEYCODE_D, .text = "d" },
                { .key = KEYCODE_D, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_NONE }
            },
        }
    },
    {
        .testpath = "/nkutils/bindings/release/combo/multi-modifiers",
        .data = {
            .bindings = {
                { .binding = "<Ctrl><Alt><Super>S", .trigger = 1 },
                { .binding = NULL }
            },
            .keys = {
                { .key = KEYCODE_A, .text = "a" },
                { .key = KEYCODE_A, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_CONTROL_L },
                { .key = KEYCODE_ALT_L },
                { .key = KEYCODE_SUPER_L },
                { .key = KEYCODE_SHIFT_L },
                { .key = KEYCODE_S, .triggered = 1 },
                { .key = KEYCODE_S, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_CONTROL_L, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_SHIFT_L, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_SUPER_L, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_ALT_L, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_D, .text = "d" },
                { .key = KEYCODE_D, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_NONE }
            },
        }
    },
    {
        .testpath = "/nkutils/bindings/press-and-release/key",
        .data = {
            .bindings = {
                { .binding = "s", .trigger = 1 },
                { .binding = "!s", .trigger = 2 },
                { .binding = NULL }
            },
            .keys = {
                { .key = KEYCODE_A, .text = "a" },
                { .key = KEYCODE_A, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_S, .triggered = 1 },
                { .key = KEYCODE_S, .state = NK_BINDINGS_KEY_STATE_RELEASE, .triggered = 2 },
                { .key = KEYCODE_D, .text = "d" },
                { .key = KEYCODE_D, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_NONE }
            },
        }
    },
#ifdef NK_XKBCOMMON_HAS_COMPOSE
    {
        .testpath = "/nkutils/bindings/compose/with-binding",
        .data = {
            .bindings = {
                { .binding = "dead_grave", .trigger = 1 },
                { .binding = NULL }
            },
            .keys = {
                { .key = KEYCODE_TILDE },
                { .key = KEYCODE_TILDE, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_TILDE, .text = "`" },
                { .key = KEYCODE_TILDE, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_NONE }
            },
        }
    },
    {
        .testpath = "/nkutils/bindings/compose/without-binding",
        .data = {
            .bindings = {
                { .binding = NULL }
            },
            .keys = {
                { .key = KEYCODE_TILDE },
                { .key = KEYCODE_TILDE, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_TILDE, .text = "`" },
                { .key = KEYCODE_TILDE, .state = NK_BINDINGS_KEY_STATE_RELEASE },
                { .key = KEYCODE_NONE }
            },
        }
    },
#endif /* NK_XKBCOMMON_HAS_COMPOSE */
};

static void
_nk_bindings_tests_setup(NkBindingsTestFixture *fixture, G_GNUC_UNUSED gconstpointer user_data)
{
    static const struct xkb_rule_names names = {
        .layout = "us",
        .variant = "intl",
    };
    fixture->bindings = nk_bindings_new(0);
    fixture->seat = nk_bindings_seat_new(fixture->bindings, XKB_CONTEXT_NO_ENVIRONMENT_NAMES);
    fixture->keymap = xkb_keymap_new_from_names(nk_bindings_seat_get_context(fixture->seat), &names, XKB_KEYMAP_COMPILE_NO_FLAGS);
    fixture->master_state = xkb_state_new(fixture->keymap);
    fixture->state = xkb_state_new(fixture->keymap);
    nk_bindings_seat_update_keymap(fixture->seat, fixture->keymap, fixture->state);
}

static gboolean
_nk_bindings_tests_callback(guint64 scope, gpointer target, gpointer user_data)
{
    NkBindingsTestBindingSlice *slice = user_data;
    g_assert_cmpuint(scope, ==, 0);
    g_assert_null(target);

    slice->fixture->triggered = slice->trigger;
    return TRUE;
}

static void
_nk_bindings_tests_func(NkBindingsTestFixture *fixture, gconstpointer user_data)
{
    NkBindingsTestData *data = (NkBindingsTestData *) user_data;
    GError *error = NULL;

    NkBindingsTestBindingSlice slices[MAX_BINDINGS], *slice = slices;
    NkBindingsTestBinding *binding;
    for ( binding = data->bindings ; binding->binding != NULL ; ++binding )
    {
        gboolean ret;

        slice->fixture = fixture;
        slice->trigger = binding->trigger;
        ret = nk_bindings_add_binding(fixture->bindings, 0, binding->binding, _nk_bindings_tests_callback, slice++, NULL, &error);

        g_assert_true(ret);
        g_assert_no_error(error);
    }

    NkBindingsTestKey *key;
    for ( key = data->keys ; key->key != KEYCODE_NONE ; ++key )
    {
        gchar *text;

        g_test_message("%s key %s", ( key->state == NK_BINDINGS_KEY_STATE_RELEASE ) ? "Releasing" : ( key->state == NK_BINDINGS_KEY_STATE_PRESSED ) ? "Already pressed" : "Pressing", _nk_bindings_test_key_names[key->key]);

        fixture->triggered = 0;
        text = nk_bindings_seat_handle_key(fixture->seat, NULL, key->key, key->state);

        if ( xkb_state_update_key(fixture->master_state, key->key, ( key->state == NK_BINDINGS_KEY_STATE_RELEASE ) ? XKB_KEY_UP : XKB_KEY_DOWN) != 0 )
        {
            g_test_message("New state is 0x%x", xkb_state_serialize_mods(fixture->master_state, XKB_STATE_MODS_DEPRESSED));
            nk_bindings_seat_update_mask(fixture->seat, NULL,
                xkb_state_serialize_mods(fixture->master_state, XKB_STATE_MODS_DEPRESSED),
                xkb_state_serialize_mods(fixture->master_state, XKB_STATE_MODS_LATCHED),
                xkb_state_serialize_mods(fixture->master_state, XKB_STATE_MODS_LOCKED),
                xkb_state_serialize_layout(fixture->master_state, XKB_STATE_LAYOUT_DEPRESSED),
                xkb_state_serialize_layout(fixture->master_state, XKB_STATE_LAYOUT_LATCHED),
                xkb_state_serialize_layout(fixture->master_state, XKB_STATE_LAYOUT_LOCKED));
        }

        g_assert_cmpstr(key->text, ==, text);
        g_assert_cmpuint(fixture->triggered, ==, key->triggered);

        g_free(text);
    }
}

static void
_nk_bindings_tests_teardown(NkBindingsTestFixture *fixture, G_GNUC_UNUSED gconstpointer user_data)
{
    nk_bindings_seat_free(fixture->seat);
    xkb_state_unref(fixture->state);
    xkb_keymap_unref(fixture->keymap);
    nk_bindings_free(fixture->bindings);
}

int
main(int argc, char *argv[])
{
    setlocale(LC_CTYPE, "en_US.UTF-8");

    g_test_init(&argc, &argv, NULL);

    g_test_set_nonfatal_assertions();

    g_setenv("HOME", "/var/empty", TRUE);

    gsize i;
    for ( i = 0 ; i < G_N_ELEMENTS(_nk_bindings_tests_list) ; ++i )
        g_test_add(_nk_bindings_tests_list[i].testpath, NkBindingsTestFixture, &_nk_bindings_tests_list[i].data, _nk_bindings_tests_setup, _nk_bindings_tests_func, _nk_bindings_tests_teardown);
    return g_test_run();
}
