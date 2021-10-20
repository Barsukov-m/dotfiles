/*
 * libnkutils/colour - Miscellaneous utilities, colour module
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

#include <nkutils-colour.h>


#define g_assert_cmpfloat_near(a, b, delta) G_STMT_START { g_assert_cmpfloat((b - delta), <, a); g_assert_cmpfloat(a, <, (b + delta)); } G_STMT_END

typedef struct {
    const gchar *string;
    const gchar *generated_string;
    const NkColour colour;
    gboolean ret;
} NkColourTestData;

static const struct {
    const gchar *testpath;
    NkColourTestData data;
} _nk_colour_tests_list[] = {
    {
        .testpath = "/nkutils/colour/null",
        .data ={
            .string = NULL,
            .ret = FALSE,
        }
    },
    {
        .testpath = "/nkutils/colour/hex/8",
        .data ={
            .string = "#ffddee7f",
            .colour = {
                .red   = 1.,
                .green = .866,
                .blue  = .933,
                .alpha = .498
            },
            .ret = TRUE,
        }
    },
    {
        .testpath = "/nkutils/colour/hex/6",
        .data ={
            .string = "#121314",
            .colour = {
                .red   = .070,
                .green = .074,
                .blue  = .078,
                .alpha = 1.
            },
            .ret = TRUE,
        }
    },
    {
        .testpath = "/nkutils/colour/hex/4",
        .data ={
            .string = "#abcd",
            .generated_string = "#aabbccdd",
            .colour = {
                .red   = .666,
                .green = .733,
                .blue  = .800,
                .alpha = .866
            },
            .ret = TRUE,
        }
    },
    {
        .testpath = "/nkutils/colour/hex/3",
        .data ={
            .string = "#369",
            .generated_string = "#336699",
            .colour = {
                .red   = .200,
                .green = .400,
                .blue  = .600,
                .alpha = 1.
            },
            .ret = TRUE,
        }
    },
    {
        .testpath = "/nkutils/colour/hex/bad",
        .data ={
            .string = "#69",
            .ret = FALSE,
        }
    },
    {
        .testpath = "/nkutils/colour/rgb",
        .data ={
            .string = "rgb(51, 102, 153)",
            .generated_string = "rgb(51.0000000000,102.0000000000,153.0000000000)",
            .colour = {
                .red   = .200,
                .green = .400,
                .blue  = .600,
                .alpha = 1.
            },
            .ret = TRUE,
        }
    },
    {
        .testpath = "/nkutils/colour/rgb/percentage",
        .data ={
            .string = "rgb(100%, 50%, 0%)",
            .generated_string = "rgb(255.0000000000,127.5000000000,0.0000000000)",
            .colour = {
                .red   = 1.,
                .green = .500,
                .blue  = 0.,
                .alpha = 1.
            },
            .ret = TRUE,
        }
    },
    {
        .testpath = "/nkutils/colour/rgba",
        .data ={
            .string = "rgba(255, 0, 127, .1)",
            .generated_string = "rgba(255.0000000000,0.0000000000,126.9900000000,0.1000000000)",
            .colour = {
                .red   = 1.,
                .green = 0.,
                .blue  = .498,
                .alpha = .1
            },
            .ret = TRUE,
        }
    },
    {
        .testpath = "/nkutils/colour/rgb/bad/1",
        .data ={
            .string = "rgb(100%, 50%, 0%",
            .ret = FALSE,
        }
    },
    {
        .testpath = "/nkutils/colour/rgb/bad/2",
        .data ={
            .string = "rgb100%, 50%, 0%",
            .ret = FALSE,
        }
    },
    {
        .testpath = "/nkutils/colour/hsl/plain",
        .data ={
            .string = "hsl(210, 50%, 50%)",
            .generated_string = "#4080bf",
            .colour = {
                .red   = .25,
                .green = .50,
                .blue  = .75,
                .alpha = 1.
            },
            .ret = TRUE,
        }
    },
    {
        .testpath = "/nkutils/colour/hsl/alpha",
        .data ={
            .string = "hsla(210, 50%, 50%, 0.75)",
            .generated_string = "#4080bfbf",
            .colour = {
                .red   = .25,
                .green = .50,
                .blue  = .75,
                .alpha = .75
            },
            .ret = TRUE,
        }
    },
    {
        .testpath = "/nkutils/colour/hwb/plain",
        .data ={
            .string = "hwb(120, 20%, 60%)",
            .generated_string = "#336633",
            .colour = {
                .red   = .2,
                .green = .4,
                .blue  = .2,
                .alpha = 1.
            },
            .ret = TRUE,
        }
    },
    {
        .testpath = "/nkutils/colour/hwb/alpha",
        .data ={
            .string = "hwb(120, 20%, 60%, 0.5)",
            .generated_string = "#33663380",
            .colour = {
                .red   = .2,
                .green = .4,
                .blue  = .2,
                .alpha = .5
            },
            .ret = TRUE,
        }
    },
    {
        .testpath = "/nkutils/colour/named",
        .data ={
            .string = "gold",
            .generated_string = "#ffd700",
            .colour = {
                .red   = 1.,
                .green = .843,
                .blue  = 0.,
                .alpha = 1.
            },
            .ret = TRUE,
        }
    },
    {
        .testpath = "/nkutils/colour/bad",
        .data ={
            .string = "no-colour-has-that-name",
            .ret = FALSE,
        }
    },
};

static void
_nk_colour_tests_func(gconstpointer user_data)
{
    const NkColourTestData *data = user_data;

    NkColour colour = { .alpha = 0 };
    gboolean r;
    r = nk_colour_parse(data->string, &colour);
    if ( data->ret )
        g_assert_true(r);
    else
        g_assert_false(r);
    g_assert_cmpfloat_near(colour.red, data->colour.red, 0.001);
    g_assert_cmpfloat_near(colour.green, data->colour.green, 0.001);
    g_assert_cmpfloat_near(colour.blue, data->colour.blue, 0.001);
    g_assert_cmpfloat_near(colour.alpha, data->colour.alpha, 0.001);

    if ( ! r )
        return;

    const gchar *string;
    const gchar *wanted_string = ( data->generated_string != NULL ) ? data->generated_string : data->string;

    if ( g_str_has_prefix(wanted_string, "rgb") )
        string = nk_colour_to_rgba(&data->colour);
    else
        string = nk_colour_to_hex(&data->colour);

    g_assert_nonnull(string);
    g_assert_cmpstr(string, ==, wanted_string);
}



int
main(int argc, char *argv[])
{
    g_test_init(&argc, &argv, NULL);

    g_test_set_nonfatal_assertions();

    gsize i;
    for ( i = 0 ; i < G_N_ELEMENTS(_nk_colour_tests_list) ; ++i )
        g_test_add_data_func(_nk_colour_tests_list[i].testpath, &_nk_colour_tests_list[i].data, _nk_colour_tests_func);

    return g_test_run();
}
