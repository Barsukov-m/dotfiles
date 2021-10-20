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
#include <locale.h>

#include <glib.h>

#include <nkutils-token.h>

#define MAX_DATA 4

typedef struct {
    const gchar *token;
    const gchar *content;
} NkTokenTestDataData;

typedef struct {
    gunichar identifier;
    const gchar *source;
    NkTokenTestDataData data[MAX_DATA + 1];
    gint error;
    const gchar *result;
} NkTokenTestData;

static const struct {
    const gchar *testpath;
    NkTokenTestData data;
} _nk_token_list_tests_list[] = {
    {
        .testpath = "/nkutils/token/basic",
        .data = {
            .identifier = '$',
            .source = "You can make ${recipe} with ${fruit}.",
            .data = {
                { .token = "fruit", .content = "'a banana'" },
                { .token = "recipe", .content = "'a banana split'" },
                { .token = NULL }
            },
            .result = "You can make a banana split with a banana."
        }
    },
    {
        .testpath = "/nkutils/token/basic/dash-underscore",
        .data = {
            .identifier = '$',
            .source = "You can make ${recipe_name} with ${fruit-name}.",
            .data = {
                { .token = "fruit-name", .content = "'a banana'" },
                { .token = "recipe_name", .content = "'a banana split'" },
                { .token = NULL }
            },
            .result = "You can make a banana split with a banana."
        }
    },
    {
        .testpath = "/nkutils/token/basic/unicode",
        .data = {
            .identifier = '$',
            .source = "You can make ${recette} with ${ingrédient}.",
            .data = {
                { .token = "ingrédient", .content = "'a banana'" },
                { .token = "recette", .content = "'a banana split'" },
                { .token = NULL }
            },
            .result = "You can make a banana split with a banana."
        }
    },
    {
        .testpath = "/nkutils/token/basic/boolean/true",
        .data = {
            .identifier = '$',
            .source = "You are ${bool}.",
            .data = {
                { .token = "bool", .content = "true" },
                { .token = NULL }
            },
            .result = "You are true."
        }
    },
    {
        .testpath = "/nkutils/token/basic/boolean/false",
        .data = {
            .identifier = '$',
            .source = "You are ${bool}.",
            .data = {
                { .token = "bool", .content = "false" },
                { .token = NULL }
            },
            .result = "You are false."
        }
    },
    {
        .testpath = "/nkutils/token/basic/wrong/1",
        .data = {
            .identifier = '$',
            .source = "You can make ${recipe} with $fruit.",
            .data = {
                { .token = "fruit", .content = "'a banana'" },
                { .token = "recipe", .content = "'a banana split'" },
                { .token = NULL }
            },
            .result = "You can make a banana split with $fruit."
        }
    },
    {
        .testpath = "/nkutils/token/basic/wrong/2",
        .data = {
            .identifier = '$',
            .source = "$fruit is good.",
            .data = {
                { .token = "fruit", .content = "'a banana'" },
                { .token = "recipe", .content = "'a banana split'" },
                { .token = NULL }
            },
            .result = "$fruit is good."
        }
    },
    {
        .testpath = "/nkutils/token/key/index/positive",
        .data = {
            .identifier = '$',
            .source = "You can make a ${recipe[0]} with ${fruit}.",
            .data = {
                { .token = "fruit", .content = "'a banana'" },
                { .token = "recipe", .content = "['banana split', 'apple pie']" },
                { .token = NULL }
            },
            .result = "You can make a banana split with a banana."
        }
    },
    {
        .testpath = "/nkutils/token/key/index/negative",
        .data = {
            .identifier = '$',
            .source = "You can make a ${recipe[-1]} with ${fruit}.",
            .data = {
                { .token = "fruit", .content = "'a banana'" },
                { .token = "recipe", .content = "['apple pie', 'banana split']" },
                { .token = NULL }
            },
            .result = "You can make a banana split with a banana."
        }
    },
    {
        .testpath = "/nkutils/token/key/name",
        .data = {
            .identifier = '$',
            .source = "You can make a ${recipe[icecream]} with ${fruit}.",
            .data = {
                { .token = "fruit", .content = "'a banana'" },
                { .token = "recipe", .content = "{'icecream': 'banana split'}" },
                { .token = NULL }
            },
            .result = "You can make a banana split with a banana."
        }
    },
    {
        .testpath = "/nkutils/token/key/name/modifier",
        .data = {
            .identifier = '$',
            .source = "You can make a ${recipe[cake]:-banana cake} with ${fruit}.",
            .data = {
                { .token = "fruit", .content = "'a banana'" },
                { .token = "recipe", .content = "{'cream': 'banana split'}" },
                { .token = NULL }
            },
            .result = "You can make a banana cake with a banana."
        }
    },
    {
        .testpath = "/nkutils/token/key/join/default",
        .data = {
            .identifier = '$',
            .source = "You can make [${recipes[@]}] with ${fruit}.",
            .data = {
                { .token = "fruit", .content = "'a banana'" },
                { .token = "recipes", .content = "['banana pie', 'banana split']" },
                { .token = NULL }
            },
            .result = "You can make [banana pie, banana split] with a banana."
        }
    },
    {
        .testpath = "/nkutils/token/key/join/custom",
        .data = {
            .identifier = '$',
            .source = "You can make [${recipes[@; ]}] with ${fruit}.",
            .data = {
                { .token = "fruit", .content = "'a banana'" },
                { .token = "recipes", .content = "['banana pie', 'banana split']" },
                { .token = NULL }
            },
            .result = "You can make [banana pie; banana split] with a banana."
        }
    },
    {
        .testpath = "/nkutils/token/key/join/replace",
        .data = {
            .identifier = '$',
            .source = "You can make [${recipes[@@]/@/], [}] with ${fruit}.",
            .data = {
                { .token = "fruit", .content = "'a banana'" },
                { .token = "recipes", .content = "['banana pie', 'banana split']" },
                { .token = NULL }
            },
            .result = "You can make [banana pie], [banana split] with a banana."
        }
    },
    {
        .testpath = "/nkutils/token/wrong/modifier",
        .data = {
            .identifier = '$',
            .source = "You can make a ${recipe} with ${fruit::}.",
            .error = NK_TOKEN_ERROR_UNKNOWN_MODIFIER,
        }
    },
    {
        .testpath = "/nkutils/token/wrong/key/index",
        .data = {
            .identifier = '$',
            .source = "You can make a ${recipe[18446744073709551616]} with ${fruit}.",
            .error = NK_TOKEN_ERROR_WRONG_KEY,
        }
    },
    {
        .testpath = "/nkutils/token/wrong/key",
        .data = {
            .identifier = '$',
            .source = "You can make a ${recipe[|]} with ${fruit}.",
            .error = NK_TOKEN_ERROR_WRONG_KEY,
        }
    },
    {
        .testpath = "/nkutils/token/wrong/regex/pattern",
        .data = {
            .identifier = '$',
            .source = "You can make a ${recipe/[} with ${fruit}.",
            .error = NK_TOKEN_ERROR_REGEX,
        }
    },
    {
        .testpath = "/nkutils/token/wrong/regex/replace",
        .data = {
            .identifier = '$',
            .source = "You can make ${recipe/a/\\gwrong} with ${fruit}.",
            .data = {
                { .token = "fruit", .content = "'a banana'" },
                { .token = "recipe", .content = "'a banana split'" },
                { .token = NULL }
            },
            .result = "You can make  with a banana."
        }
    },
    {
        .testpath = "/nkutils/token/fallback/with",
        .data = {
            .identifier = '$',
            .source = "I want to eat ${fruit:-an apple}.",
            .data = {
                { .token = "fruit", .content = "'a banana'" },
                { .token = NULL }
            },
            .result = "I want to eat a banana."
        }
    },
    {
        .testpath = "/nkutils/token/fallback/without",
        .data = {
            .identifier = '$',
            .source = "I want to eat ${fruit:-an apple}.",
            .data = {
                { .token = NULL }
            },
            .result = "I want to eat an apple."
        }
    },
    {
        .testpath = "/nkutils/token/fallback/boolean/true",
        .data = {
            .identifier = '$',
            .source = "I want to eat a ${good:-bad} banana.",
            .data = {
                { .token = "good", .content = "true" },
                { .token = NULL }
            },
            .result = "I want to eat a true banana."
        }
    },
    {
        .testpath = "/nkutils/token/fallback/boolean/false",
        .data = {
            .identifier = '$',
            .source = "I want to eat a ${good:-bad} banana.",
            .data = {
                { .token = "good", .content = "false" },
                { .token = NULL }
            },
            .result = "I want to eat a bad banana."
        }
    },
    {
        .testpath = "/nkutils/token/fallback/recurse",
        .data = {
            .identifier = '$',
            .source = "I want to eat ${fruit:-${vegetable}}.",
            .data = {
                { .token = "vegetable", .content = "'a zucchini'" },
                { .token = NULL }
            },
            .result = "I want to eat a zucchini."
        }
    },
    {
        .testpath = "/nkutils/token/substitute/with",
        .data = {
            .identifier = '$',
            .source = "You can make a ${adjective:+(}${adjective}${adjective:+) }${recipe} with ${fruit}${addition:+ and }${addition}.",
            .data = {
                { .token = "adjective", .content = "'creamy'" },
                { .token = "fruit", .content = "'a banana'" },
                { .token = "recipe", .content = "'banana split'" },
                { .token = "addition", .content = "'some cream'" },
                { .token = NULL }
            },
            .result = "You can make a (creamy) banana split with a banana and some cream."
        }
    },
    {
        .testpath = "/nkutils/token/substitute/without",
        .data = {
            .identifier = '$',
            .source = "You can make a ${adjective:+(}${adjective}${adjective:+) }${recipe} with ${fruit}${addition:+ and }${addition}.",
            .data = {
                { .token = "fruit", .content = "'a banana'" },
                { .token = "recipe", .content = "'banana split'" },
                { .token = NULL }
            },
            .result = "You can make a banana split with a banana."
        }
    },
    {
        .testpath = "/nkutils/token/substitute/boolean/true",
        .data = {
            .identifier = '$',
            .source = "You can make a ${good:+good }${recipe} with ${fruit}${addition:+ and }${addition}.",
            .data = {
                { .token = "good", .content = "true" },
                { .token = "fruit", .content = "'a banana'" },
                { .token = "recipe", .content = "'banana split'" },
                { .token = NULL }
            },
            .result = "You can make a good banana split with a banana."
        }
    },
    {
        .testpath = "/nkutils/token/substitute/boolean/false",
        .data = {
            .identifier = '$',
            .source = "You can make a ${good:+good }${recipe} with ${fruit}${addition:+ and }${addition}.",
            .data = {
                { .token = "good", .content = "false" },
                { .token = "fruit", .content = "'a banana'" },
                { .token = "recipe", .content = "'banana split'" },
                { .token = NULL }
            },
            .result = "You can make a banana split with a banana."
        }
    },
    {
        .testpath = "/nkutils/token/anti-substitute/with",
        .data = {
            .identifier = '$',
            .source = "I want to eat a ${adjective:!sweat }lemon.",
            .data = {
                { .token = "adjective", .content = "'juicy'" },
                { .token = NULL }
            },
            .result = "I want to eat a lemon."
        }
    },
    {
        .testpath = "/nkutils/token/anti-substitute/without",
        .data = {
            .identifier = '$',
            .source = "I want to eat a ${adjective:!sweat }lemon.",
            .data = {
                { .token = NULL }
            },
            .result = "I want to eat a sweat lemon."
        }
    },
    {
        .testpath = "/nkutils/token/anti-substitute/boolean/true",
        .data = {
            .identifier = '$',
            .source = "I want to eat a ${good:!bad }lemon.",
            .data = {
                { .token = "good", .content = "true" },
                { .token = NULL }
            },
            .result = "I want to eat a lemon."
        }
    },
    {
        .testpath = "/nkutils/token/anti-substitute/boolean/false",
        .data = {
            .identifier = '$',
            .source = "I want to eat a ${good:!bad }lemon.",
            .data = {
                { .token = "good", .content = "false" },
                { .token = NULL }
            },
            .result = "I want to eat a bad lemon."
        }
    },
    {
        .testpath = "/nkutils/token/switch/true",
        .data = {
            .identifier = '$',
            .source = "Active: ${active:{;yes;no}}.",
            .data = {
                { .token = "active", .content = "true" },
                { .token = NULL }
            },
            .result = "Active: yes."
        }
    },
    {
        .testpath = "/nkutils/token/switch/false",
        .data = {
            .identifier = '$',
            .source = "Active: ${active:{;yes;no}}.",
            .data = {
                { .token = "active", .content = "false" },
                { .token = NULL }
            },
            .result = "Active: no."
        }
    },
    {
        .testpath = "/nkutils/token/range/symbol",
        .data = {
            .identifier = '$',
            .source = "Dice roll gave: ${dice:[;1;byte 6;⚀;⚁;⚂;⚃;⚄;⚅]}.",
            .data = {
                { .token = "dice", .content = "uint64 6" },
                { .token = NULL }
            },
            .result = "Dice roll gave: ⚅."
        }
    },
    {
        .testpath = "/nkutils/token/range/text",
        .data = {
            .identifier = '$',
            .source = "Signal strength: ${signal:[;0;100;low;medium;high;full]}.",
            .data = {
                { .token = "signal", .content = "24" },
                { .token = NULL }
            },
            .result = "Signal strength: low."
        }
    },
    {
        .testpath = "/nkutils/token/range/plural/singular",
        .data = {
            .identifier = '$',
            .source = "${quantity} unit${quantity:[;2;2;;s]}",
            .data = {
                { .token = "quantity", .content = "1" },
                { .token = NULL }
            },
            .result = "1 unit"
        }
    },
    {
        .testpath = "/nkutils/token/range/plural/plural",
        .data = {
            .identifier = '$',
            .source = "${quantity} unit${quantity:[;2;2;;s]}",
            .data = {
                { .token = "quantity", .content = "2" },
                { .token = NULL }
            },
            .result = "2 units"
        }
    },
    {
        .testpath = "/nkutils/token/range/middle-split",
        .data = {
            .identifier = '$',
            .source = "Signal strength: ${signal:[;0;100;bad;good]}.",
            .data = {
                { .token = "signal", .content = "50" },
                { .token = NULL }
            },
            .result = "Signal strength: good."
        }
    },
    {
        .testpath = "/nkutils/token/range/double",
        .data = {
            .identifier = '$',
            .source = "${coin:[;0.0;1.0;heads;tails]}",
            .data = {
                { .token = "coin", .content = "0.6" },
                { .token = NULL }
            },
            .result = "tails"
        }
    },
    {
        .testpath = "/nkutils/token/prettify/float/width",
        .data = {
            .identifier = '$',
            .source = "${value(f4)}",
            .data = {
                { .token = "value", .content = "1" },
                { .token = NULL }
            },
            .result = "   1"
        }
    },
    {
        .testpath = "/nkutils/token/prettify/float/0-padding",
        .data = {
            .identifier = '$',
            .source = "${value(f04)}",
            .data = {
                { .token = "value", .content = "1" },
                { .token = NULL }
            },
            .result = "0001"
        }
    },
    {
        .testpath = "/nkutils/token/prettify/float/precision",
        .data = {
            .identifier = '$',
            .source = "${value(f.5)}",
            .data = {
                { .token = "value", .content = "1" },
                { .token = NULL }
            },
            .result = "1.00000"
        }
    },
    {
        .testpath = "/nkutils/token/prettify/prefixes/si/big",
        .data = {
            .identifier = '$',
            .source = "${value(p)}",
            .data = {
                { .token = "value", .content = "1000000" },
                { .token = NULL }
            },
            .result = "1M"
        }
    },
    {
        .testpath = "/nkutils/token/prettify/prefixes/si/small",
        .data = {
            .identifier = '$',
            .source = "${value(p)}",
            .data = {
                { .token = "value", .content = "0.001" },
                { .token = NULL }
            },
            .result = "1m"
        }
    },
    {
        .testpath = "/nkutils/token/prettify/prefixes/si/zero",
        .data = {
            .identifier = '$',
            .source = "${value(p)}",
            .data = {
                { .token = "value", .content = "0" },
                { .token = NULL }
            },
            .result = "0"
        }
    },
    {
        .testpath = "/nkutils/token/prettify/prefixes/si/with-precision",
        .data = {
            .identifier = '$',
            .source = "${value(p.1)}",
            .data = {
                { .token = "value", .content = "1000000" },
                { .token = NULL }
            },
            .result = "1.0M"
        }
    },
    {
        .testpath = "/nkutils/token/prettify/prefixes/si/2-precision",
        .data = {
            .identifier = '$',
            .source = "${value(p.2)}",
            .data = {
                { .token = "value", .content = "626704" },
                { .token = NULL }
            },
            .result = "626.70k"
        }
    },
    {
        .testpath = "/nkutils/token/prettify/prefixes/si/0-precision",
        .data = {
            .identifier = '$',
            .source = "${value(p.0)}",
            .data = {
                { .token = "value", .content = "626704" },
                { .token = NULL }
            },
            .result = "627k"
        }
    },
    {
        .testpath = "/nkutils/token/prettify/prefixes/binary/big",
        .data = {
            .identifier = '$',
            .source = "${value(b)}",
            .data = {
                { .token = "value", .content = "626704" },
                { .token = NULL }
            },
            .result = "612.015625Ki"
        }
    },
    {
        .testpath = "/nkutils/token/prettify/prefixes/binary/zero",
        .data = {
            .identifier = '$',
            .source = "${value(b)}",
            .data = {
                { .token = "value", .content = "0" },
                { .token = NULL }
            },
            .result = "0"
        }
    },
    {
        .testpath = "/nkutils/token/prettify/time/default",
        .data = {
            .identifier = '$',
            .source = "${timestamp(t)}",
            .data = {
                { .token = "timestamp", .content = "1519910048" },
                { .token = NULL }
            },
            .result = "Thu Mar  1 13:14:08 2018"
        }
    },
    {
        .testpath = "/nkutils/token/prettify/time/with-format",
        .data = {
            .identifier = '$',
            .source = "${timestamp(t%F %T)}",
            .data = {
                { .token = "timestamp", .content = "1519910048" },
                { .token = NULL }
            },
            .result = "2018-03-01 13:14:08"
        }
    },
    {
        .testpath = "/nkutils/token/prettify/duration/default",
        .data = {
            .identifier = '$',
            .source = "${duration(d)}",
            .data = {
                { .token = "duration", .content = "788645" },
                { .token = NULL }
            },
            .result = "1 week 2 days 3 hours 4 minutes 5 seconds"
        }
    },
    {
        .testpath = "/nkutils/token/prettify/duration/with-format",
        .data = {
            .identifier = '$',
            .source = "${duration(d%{weeks}w %{days}d %{hours(f02)}:%{minutes(f02)}:%{seconds(f02)})}",
            .data = {
                { .token = "duration", .content = "788645" },
                { .token = NULL }
            },
            .result = "1w 2d 03:04:05"
        }
    },
    {
        .testpath = "/nkutils/token/replace/full",
        .data = {
            .identifier = '$',
            .source = "You can make a ${recipe/split/cream} with ${fruit}.",
            .data = {
                { .token = "fruit", .content = "'a banana'" },
                { .token = "recipe", .content = "'banana split'" },
                { .token = NULL }
            },
            .result = "You can make a banana cream with a banana."
        }
    },
    {
        .testpath = "/nkutils/token/replace/missing",
        .data = {
            .identifier = '$',
            .source = "You can make a ${recipe} with ${fruit}${addition/^/ and }.",
            .data = {
                { .token = "fruit", .content = "'a banana'" },
                { .token = "recipe", .content = "'banana split'" },
                { .token = NULL }
            },
            .result = "You can make a banana split with a banana."
        }
    },
    {
        .testpath = "/nkutils/token/replace/capture",
        .data = {
            .identifier = '$',
            .source = "You can make a ${adjective/(.+)/(\\1) }${recipe} with ${fruit}${addition/^/ and }.",
            .data = {
                { .token = "adjective", .content = "'creamy'" },
                { .token = "fruit", .content = "'a banana'" },
                { .token = "recipe", .content = "'banana split'" },
                { .token = "addition", .content = "'some cream'" },
                { .token = NULL }
            },
            .result = "You can make a (creamy) banana split with a banana and some cream."
        }
    },
    {
        .testpath = "/nkutils/token/replace/before-after/with",
        .data = {
            .identifier = '$',
            .source = "You can make a ${adjective/^/(/$/) }${recipe} with ${fruit}${addition/^/ and }.",
            .data = {
                { .token = "adjective", .content = "'creamy'" },
                { .token = "fruit", .content = "'a banana'" },
                { .token = "recipe", .content = "'banana split'" },
                { .token = "addition", .content = "'some cream'" },
                { .token = NULL }
            },
            .result = "You can make a (creamy) banana split with a banana and some cream."
        }
    },
    {
        .testpath = "/nkutils/token/replace/before-after/without",
        .data = {
            .identifier = '$',
            .source = "You can make a ${adjective/^/(/$/) }${recipe} with ${fruit}${addition/^/ and }.",
            .data = {
                { .token = "fruit", .content = "'a banana'" },
                { .token = "recipe", .content = "'banana split'" },
                { .token = NULL }
            },
            .result = "You can make a banana split with a banana."
        }
    },
    {
        .testpath = "/nkutils/token/replace/remove",
        .data = {
            .identifier = '$',
            .source = "You can make a ${recipe/ split} with ${fruit}.",
            .data = {
                { .token = "fruit", .content = "'a banana'" },
                { .token = "recipe", .content = "'banana split'" },
                { .token = NULL }
            },
            .result = "You can make a banana with a banana."
        }
    },
    {
        .testpath = "/nkutils/token/replace/multiple",
        .data = {
            .identifier = '$',
            .source = "You can make ${recipe/a banana/an apple pie/ split} with ${fruit/.+/apples}.",
            .data = {
                { .token = "fruit", .content = "'a banana'" },
                { .token = "recipe", .content = "'a banana split'" },
                { .token = NULL }
            },
            .result = "You can make an apple pie with apples."
        }
    },
    {
        .testpath = "/nkutils/token/replace/braces/paired",
        .data = {
            .identifier = '$',
            .source = "You can make a ${adjective/.{2}$/y/^/(/$/) }${recipe} with ${fruit}${addition/\\{//\\}//^/ and }.",
            .data = {
                { .token = "adjective", .content = "'creamed'" },
                { .token = "fruit", .content = "'a banana'" },
                { .token = "recipe", .content = "'banana split'" },
                { .token = "addition", .content = "'some cream{}'" },
                { .token = NULL }
            },
            .result = "You can make a (creamy) banana split with a banana and some cream."
        }
    },
    {
        .testpath = "/nkutils/token/replace/braces/opening",
        .data = {
            .identifier = '$',
            .source = "You can make a ${adjective/.{2}$/y/^/(/$/) }${recipe} with ${fruit}${addition/\\{//^/ and }.",
            .data = {
                { .token = "adjective", .content = "'creamed'" },
                { .token = "fruit", .content = "'a banana'" },
                { .token = "recipe", .content = "'banana split'" },
                { .token = "addition", .content = "'some cream{'" },
                { .token = NULL }
            },
            .result = "You can make a (creamy) banana split with a banana and some cream."
        }
    },
    {
        .testpath = "/nkutils/token/replace/braces/closing",
        .data = {
            .identifier = '$',
            .source = "You can make a ${adjective/.{2}$/y/^/(/$/) }${recipe} with ${fruit}${addition/\\}//^/ and }.",
            .data = {
                { .token = "adjective", .content = "'creamed'" },
                { .token = "fruit", .content = "'a banana'" },
                { .token = "recipe", .content = "'banana split'" },
                { .token = "addition", .content = "'some cream}'" },
                { .token = NULL }
            },
            .result = "You can make a (creamy) banana split with a banana and some cream."
        }
    },
    {
        .testpath = "/nkutils/token/replace/escaping/backslash",
        .data = {
            .identifier = '$',
            .source = "You can make a ${adjective/^/(/$/) /\\\\}${recipe} with ${fruit}${addition/^/ and }.",
            .data = {
                { .token = "adjective", .content = "'creamy\\\\'" },
                { .token = "fruit", .content = "'a banana'" },
                { .token = "recipe", .content = "'banana split'" },
                { .token = "addition", .content = "'some cream'" },
                { .token = NULL }
            },
            .result = "You can make a (creamy) banana split with a banana and some cream."
        }
    },
    {
        .testpath = "/nkutils/token/replace/escaping/forwardslash/1",
        .data = {
            .identifier = '$',
            .source = "${data/\\/}",
            .data = {
                { .token = "data", .content = "'/'" },
                { .token = NULL }
            },
            .result = ""
        }
    },
    {
        .testpath = "/nkutils/token/replace/escaping/forwardslash/2",
        .data = {
            .identifier = '$',
            .source = "${data/a/\\/}",
            .data = {
                { .token = "data", .content = "'a'" },
                { .token = NULL }
            },
            .result = "/"
        }
    },
    {
        .testpath = "/nkutils/token/replace/escaping/forwardslash/3",
        .data = {
            .identifier = '$',
            .source = "${data/a/\\//b/x}",
            .data = {
                { .token = "data", .content = "'ab'" },
                { .token = NULL }
            },
            .result = "/x"
        }
    },
    {
        .testpath = "/nkutils/token/replace/escaping/right-curly-bracket/2",
        .data = {
            .identifier = '$',
            .source = "${data/a/\\}}",
            .data = {
                { .token = "data", .content = "'a'" },
                { .token = NULL }
            },
            .result = "}"
        }
    },
    {
        .testpath = "/nkutils/token/replace/recurse/with",
        .data = {
            .identifier = '$',
            .source = "I want to eat ${recipe/an apple/${fruit}}.",
            .data = {
                { .token = "recipe", .content = "'an apple pie'" },
                { .token = "fruit", .content = "'a blackberry'" },
                { .token = NULL }
            },
            .result = "I want to eat a blackberry pie."
        }
    },
    {
        .testpath = "/nkutils/token/old/before-after",
        .data = {
            .identifier = '$',
            .source = "You can make a ${(<adjective>) }${recipe} with ${fruit}${ and <addition}.",
            .data = {
                { .token = "fruit", .content = "'a banana'" },
                { .token = "recipe", .content = "'banana split'" },
                { .token = NULL }
            },
            .result = "You can make a ${(<adjective>) }banana split with a banana${ and <addition}."
        }
    },
    {
        .testpath = "/nkutils/token/identifier/double-escape",
        .data = {
            .identifier = '$',
            .source = "echo $${PATH}",
            .data = {
                { .token = NULL }
            },
            .result = "echo ${PATH}"
        }
    },
    {
        .testpath = "/nkutils/token/identifier/non-dollar",
        .data = {
            .identifier = '%',
            .source = "Some %{variable}",
            .data = {
                { .token = "variable", .content = "'value'" },
                { .token = NULL }
            },
            .result = "Some value"
        }
    },
    {
        .testpath = "/nkutils/token/identifier/none",
        .data = {
            .identifier = '\0',
            .source = "Some {variable}",
            .data = {
                { .token = "variable", .content = "'value'" },
                { .token = NULL }
            },
            .result = "Some value"
        }
    },
};

static GVariant *
_nk_token_list_tests_callback(const gchar *token, guint64 value, gpointer user_data)
{
    NkTokenTestData *test_data = user_data;
    NkTokenTestDataData *data;
    g_assert_cmpuint(value, ==, 0);
    for ( data = test_data->data ; data->token != NULL ; ++data )
    {
        if ( g_strcmp0(token, data->token) == 0 )
            return g_variant_parse(NULL, data->content, NULL, NULL, NULL);
    }
    return NULL;
}

static void
_nk_token_list_tests_func(gconstpointer user_data)
{
    NkTokenTestData *data = (NkTokenTestData *) user_data;
    NkTokenList *token_list;
    GError *error = NULL;

    token_list = nk_token_list_parse(g_strdup(data->source), data->identifier, &error);
    if ( data->result == NULL )
    {
        g_assert_null(token_list);
        g_assert_error(error, NK_TOKEN_ERROR, data->error);
        return;
    }
    g_assert_nonnull(token_list);
    g_assert_no_error(error);
    g_assert_nonnull(nk_token_list_ref(token_list));

    gchar *result;
    result = nk_token_list_replace(token_list, _nk_token_list_tests_callback, data);

    g_assert_cmpstr(result, ==, data->result);

    nk_token_list_unref(token_list);
    nk_token_list_unref(token_list);
}

typedef enum {
    TOKEN_FRUIT,
    TOKEN_RECIPE,
    TOKEN_VALUE,
    _TOKEN_SIZE
} NkTokenListEnumTokens;

static const gchar * const _nk_token_list_enum_tests_tokens[_TOKEN_SIZE] = {
    [TOKEN_FRUIT]  = "fruit",
    [TOKEN_RECIPE] = "recipe",
    [TOKEN_VALUE] = "value",
};

typedef struct {
    gunichar identifier;
    const gchar *source;
    gchar *data[_TOKEN_SIZE];
    guint64 used_tokens;
    gint error;
    const gchar *result;
} NkTokenListEnumTestData;

static const struct {
    const gchar *testpath;
    NkTokenListEnumTestData data;
} _nk_token_list_enum_tests_list[] = {
    {
        .testpath = "/nkutils/token/enum/basic",
        .data = {
            .identifier = '$',
            .source = "You can make ${recipe} with ${fruit}.",
            .data = {
                [TOKEN_FRUIT]  = "'a banana'",
                [TOKEN_RECIPE] = "'a banana split'",
            },
            .used_tokens = (1 << TOKEN_FRUIT) | (1 << TOKEN_RECIPE),
            .result = "You can make a banana split with a banana."
        }
    },
    {
        .testpath = "/nkutils/token/enum/nested",
        .data = {
            .identifier = '$',
            .source = "I want to eat ${recipe:+${fruit} ${recipe}}.",
            .data = {
                [TOKEN_FRUIT]  = "'an apple'",
                [TOKEN_RECIPE] = "'pie'",
            },
            .used_tokens = (1 << TOKEN_FRUIT) | (1 << TOKEN_RECIPE),
            .result = "I want to eat an apple pie."
        }
    },
    {
        .testpath = "/nkutils/token/enum/fallback",
        .data = {
            .identifier = '$',
            .source = "I want to eat ${fruit} ${recipe:-pie}.",
            .data = {
                [TOKEN_FRUIT]  = "'an apple'",
            },
            .used_tokens = (1 << TOKEN_FRUIT) | (1 << TOKEN_RECIPE),
            .result = "I want to eat an apple pie."
        }
    },
    {
        .testpath = "/nkutils/token/enum/substitute",
        .data = {
            .identifier = '$',
            .source = "I want to eat ${fruit:+a fruit}.",
            .data = {
                [TOKEN_FRUIT]  = "'an apple'",
            },
            .used_tokens = (1 << TOKEN_FRUIT),
            .result = "I want to eat a fruit."
        }
    },
    {
        .testpath = "/nkutils/token/enum/anti-substitute",
        .data = {
            .identifier = '$',
            .source = "I want to eat${fruit:! a fruit}.",
            .data = {
                [TOKEN_FRUIT]  = "'an apple'",
            },
            .used_tokens = (1 << TOKEN_FRUIT),
            .result = "I want to eat."
        }
    },
    {
        .testpath = "/nkutils/token/enum/prettify/float",
        .data = {
            .identifier = '$',
            .source = "${value(f.2)}",
            .data = {
                [TOKEN_VALUE]  = "1.5555",
            },
            .used_tokens = (1 << TOKEN_VALUE),
            .result = "1.56"
        }
    },
    {
        .testpath = "/nkutils/token/enum/prettify/prefixes/si",
        .data = {
            .identifier = '$',
            .source = "${value(p)}",
            .data = {
                [TOKEN_VALUE]  = "1000",
            },
            .used_tokens = (1 << TOKEN_VALUE),
            .result = "1k"
        }
    },
    {
        .testpath = "/nkutils/token/enum/prettify/prefixes/binary",
        .data = {
            .identifier = '$',
            .source = "${value(b)}",
            .data = {
                [TOKEN_VALUE]  = "1024",
            },
            .used_tokens = (1 << TOKEN_VALUE),
            .result = "1Ki"
        }
    },
    {
        .testpath = "/nkutils/token/enum/regex",
        .data = {
            .identifier = '$',
            .source = "I want to eat ${fruit/an apple/a banana} ${recipe}.",
            .data = {
                [TOKEN_FRUIT]  = "'an apple'",
                [TOKEN_RECIPE]  = "'split'",
            },
            .used_tokens = (1 << TOKEN_FRUIT) | (1 << TOKEN_RECIPE),
            .result = "I want to eat a banana split."
        }
    },
    {
        .testpath = "/nkutils/token/enum/wrong/regex",
        .data = {
            .identifier = '$',
            .source = "You can make a ${recipe/[} with ${fruit}.",
            .error = NK_TOKEN_ERROR_REGEX,
        }
    },
    {
        .testpath = "/nkutils/token/enum/wrong/token",
        .data = {
            .identifier = '$',
            .source = "You can make a ${recipe} with ${fruit} and ${addition}.",
            .error = NK_TOKEN_ERROR_UNKNOWN_TOKEN,
        }
    },
};

static GVariant *
_nk_token_list_enum_tests_callback(const gchar *token, guint64 value, gpointer user_data)
{
    const gchar * const *data = user_data;
    g_assert_cmpstr(token, ==, _nk_token_list_enum_tests_tokens[value]);
    if ( data[value] == NULL )
        return NULL;
    return g_variant_parse(NULL, data[value], NULL, NULL, NULL);
}

static void
_nk_token_list_enum_tests_func(gconstpointer user_data)
{
    NkTokenListEnumTestData *data = (NkTokenListEnumTestData *) user_data;
    NkTokenList *token_list;
    guint64 used_tokens;
    GError *error = NULL;

    token_list = nk_token_list_parse_enum(g_strdup(data->source), data->identifier, _nk_token_list_enum_tests_tokens, _TOKEN_SIZE, &used_tokens, &error);
    if ( data->result == NULL )
    {
        g_assert_null(token_list);
        g_assert_error(error, NK_TOKEN_ERROR, data->error);
        return;
    }
    g_assert_no_error(error);
    g_assert_nonnull(token_list);
    if ( data->used_tokens != 0 )
        g_assert_cmpuint(used_tokens, ==, data->used_tokens);

    gchar *result;
    result = nk_token_list_replace(token_list, _nk_token_list_enum_tests_callback, data->data);

    g_assert_cmpstr(result, ==, data->result);

    nk_token_list_unref(token_list);
}

int
main(int argc, char *argv[])
{
    setlocale(LC_ALL, "C");

    g_setenv("LANG", "C", TRUE);
    g_setenv("TZ", "UTC", TRUE);

    g_test_init(&argc, &argv, NULL);

    g_test_set_nonfatal_assertions();

    gsize i;
    for ( i = 0 ; i < G_N_ELEMENTS(_nk_token_list_tests_list) ; ++i )
        g_test_add_data_func(_nk_token_list_tests_list[i].testpath, &_nk_token_list_tests_list[i].data, _nk_token_list_tests_func);

    for ( i = 0 ; i < G_N_ELEMENTS(_nk_token_list_enum_tests_list) ; ++i )
        g_test_add_data_func(_nk_token_list_enum_tests_list[i].testpath, &_nk_token_list_enum_tests_list[i].data, _nk_token_list_enum_tests_func);

    return g_test_run();
}
