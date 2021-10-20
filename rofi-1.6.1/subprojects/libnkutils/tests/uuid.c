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

#include <glib.h>

#include <nkutils-uuid.h>

typedef struct {
    const gchar *ns;
    const gchar *name;
    const gchar *result;
} NkUuidTestData;

static const struct {
    const gchar *testpath;
    NkUuidTestData data;
} _nk_uuid_tests_list[] = {
    {
        .testpath = "/nkutils/uuid/v5/1",
        .data = {
            .ns = "02ec50af-981d-4ec3-b520-d395abe89653",
            .name = "test-domain.example",
            /* The hash result conflicts with version */
            .result = "625fe677-3039-521b-a02c-aeb990651e07"
        }
    },
    {
        .testpath = "/nkutils/uuid/v5/2",
        .data = {
            .ns = "c14acc86-fecd-435c-8fc8-09b14ac22f5b",
            .name = "example.org",
            /* The hash result conflicts with both variant and version */
            .result = "6c238dbe-3010-551e-89c3-8ef98d6f4d1b"
        }
    },
};

static void
_nk_uuid_tests_parse_func(void)
{
    NkUuid uuid = NK_UUID_INIT, uuid2 = NK_UUID_INIT;

    nk_uuid_generate(&uuid);
    g_assert_true(nk_uuid_parse(&uuid2, uuid.string));
    g_assert_cmpstr(uuid.string, ==, uuid2.string);
}

static void
_nk_uuid_tests_fail_func(gconstpointer user_data)
{
    const gchar *str = user_data;
    NkUuid uuid = NK_UUID_INIT;

    g_assert_false(nk_uuid_parse(&uuid, str));
}

static void
_nk_uuid_tests_ns_func(gconstpointer user_data)
{
    const NkUuidTestData *data = user_data;

    NkUuid uuid = NK_UUID_INIT;
    g_assert_true(nk_uuid_parse(&uuid, data->ns));
    g_assert_cmpstr(uuid.string, ==, data->ns);
    nk_uuid_from_name(&uuid, data->name, -1);
    g_assert_cmpstr(uuid.string, ==, data->result);
}

int
main(int argc, char *argv[])
{
    g_test_init(&argc, &argv, NULL);

    g_test_set_nonfatal_assertions();

    g_test_add_func("/nkutils/uuui/generation", _nk_uuid_tests_parse_func);
    g_test_add_data_func("/nkutils/uuui/parse/fail", "z0c246e98-6678-49b1-bf28-82f383012e86", _nk_uuid_tests_fail_func);

    gsize i;
    for ( i = 0 ; i < G_N_ELEMENTS(_nk_uuid_tests_list) ; ++i )
        g_test_add_data_func(_nk_uuid_tests_list[i].testpath, &_nk_uuid_tests_list[i].data, _nk_uuid_tests_ns_func);

    return g_test_run();
}
