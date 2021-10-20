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

#ifdef G_LOG_DOMAIN
#undef G_LOG_DOMAIN
#endif /* G_LOG_DOMAIN */
#define G_LOG_DOMAIN "libnkutils-token"

#include <string.h>
#include <errno.h>

#include <glib.h>

#include "nkutils-enum.h"

#include "nkutils-token.h"

#define NK_TOKEN_PRETTIFY_DURATION_DEFAULT "%{weeks:+%{weeks} week%{weeks:[;2;2;;s]} }%{days:+%{days} day%{days:[;2;2;;s]} }%{hours:+%{hours} hour%{hours:[;2;2;;s]} }%{minutes:+%{minutes} minute%{minutes:[;2;2;;s]} }%{seconds:-0} second%{seconds:[;2;2;;s]}"

typedef struct {
    gdouble min;
    gdouble max;
    gsize length;
    gchar **values;
} NkTokenRange;

typedef struct {
    gchar *true_;
    gchar *false_;
} NkTokenSwitch;

typedef enum {
    NK_TOKEN_PRETTIFY_NONE = 0,
    NK_TOKEN_PRETTIFY_FLOAT = 'f',
    NK_TOKEN_PRETTIFY_PREFIXES_SI = 'p',
    NK_TOKEN_PRETTIFY_PREFIXES_BINARY = 'b',
    NK_TOKEN_PRETTIFY_TIME = 't',
    NK_TOKEN_PRETTIFY_DURATION = 'd',
} NkTokenPrettifyType;

typedef struct {
    NkTokenPrettifyType type;
    gchar format[10]; /* %0*.*lf%s + \0 */
    const gchar *time_format;
    NkTokenList *duration_format;
    gint width;
    gint precision;
} NkTokenPrettify;

typedef enum {
    NK_TOKEN_PRETTIFY_DURATION_TOKEN_WEEKS,
    NK_TOKEN_PRETTIFY_DURATION_TOKEN_DAYS,
    NK_TOKEN_PRETTIFY_DURATION_TOKEN_HOURS,
    NK_TOKEN_PRETTIFY_DURATION_TOKEN_MINUTES,
    NK_TOKEN_PRETTIFY_DURATION_TOKEN_SECONDS,
    NK_TOKEN_PRETTIFY_DURATION_TOKEN_MILLISECONDS,
    NK_TOKEN_PRETTIFY_DURATION_TOKEN_MICROSECONDS,
    NK_TOKEN_PRETTIFY_DURATION_TOKEN_NANOSECONDS,
} NkTokenListPrettifyDurationToken;

static const gchar * const _nk_token_list_prettify_duration_tokens[] = {
    [NK_TOKEN_PRETTIFY_DURATION_TOKEN_WEEKS] = "weeks",
    [NK_TOKEN_PRETTIFY_DURATION_TOKEN_DAYS] = "days",
    [NK_TOKEN_PRETTIFY_DURATION_TOKEN_HOURS] = "hours",
    [NK_TOKEN_PRETTIFY_DURATION_TOKEN_MINUTES] = "minutes",
    [NK_TOKEN_PRETTIFY_DURATION_TOKEN_SECONDS] = "seconds",
    [NK_TOKEN_PRETTIFY_DURATION_TOKEN_MILLISECONDS] = "milliseconds",
    [NK_TOKEN_PRETTIFY_DURATION_TOKEN_MICROSECONDS] = "microseconds",
    [NK_TOKEN_PRETTIFY_DURATION_TOKEN_NANOSECONDS] = "nanoseconds",
};

typedef struct {
    guint64 w;
    guint8 d, h, m, s;
    guint16 ms, us, ns;
} NkTokenListPrettifyDurationData;

typedef struct {
    GRegex *regex;
    NkTokenList *replacement;
} NkTokenRegex;

typedef struct {
    const gchar *string;
    const gchar *name;
    const gchar *key;
    gint64 index;
    guint64 value;
    NkTokenList *fallback;
    NkTokenList *substitute;
    NkTokenRange range;
    NkTokenSwitch switch_;
    NkTokenPrettify prettify;
    NkTokenRegex *replace;
    gboolean no_data;
} NkToken;

struct _NkTokenList {
    guint64 ref_count;
    gboolean owned;
    gchar *string;
    gsize length;
    NkToken *tokens;
    gsize size;
};


GQuark
nk_token_error_quark(void)
{
    return g_quark_from_static_string("nk_token_error-quark");
}

static gchar *
_nk_token_strchr_escape(gchar *s, gsize l, gunichar c, gunichar pair_c)
{
    gchar *e = s + l;

    gsize pair_count = 0;
    gchar *w = s;
    gunichar wc = '\0', pc;

    for ( ; w < e ; w = g_utf8_next_char(w) )
    {
        pc = wc;
        wc = g_utf8_get_char(w);

        if ( pc == '\\' )
        {
            /* Escaped, search for next one */
            if ( wc == '\\' )
                /* Escaping a backslash, avoid escaping the next char */
                wc = '\0';
            else if ( wc == c )
            {
                w = g_utf8_prev_char(w);
                pc = wc;
                gchar *to, *from;
                for ( to = w, from = g_utf8_next_char(w) ; from <= e ; ++to, ++from )
                    *to = *from;
                e = to;
            }
            continue;
        }

        /* Maybe do we open a paired character */
        if ( ( pair_c != '\0' ) && ( wc == pair_c ) )
            ++pair_count;

        if ( wc != c )
            continue;

        /* We found our character, check if it is the right occurence */

        if ( ( pair_c != '\0' ) && ( pair_count > 0 ) )
        {
            /* We had an opened pair, close it */
            --pair_count;
            continue;
        }

        return w;
    }
    return NULL;
}

static gboolean
_nk_token_double_from_variant(GVariant *var, gdouble *value, GError **error)
{
#define _nk_token_list_range_value_check_type(t, T, s) \
    else if ( g_variant_is_of_type(var, G_VARIANT_TYPE_##T##s) ) \
        *value = g_variant_get_##t##s(var)

    if ( g_variant_is_of_type(var, G_VARIANT_TYPE_DOUBLE) )
        *value = g_variant_get_double(var);
    _nk_token_list_range_value_check_type(int, INT, 16);
    _nk_token_list_range_value_check_type(int, INT, 32);
    _nk_token_list_range_value_check_type(int, INT, 64);
    _nk_token_list_range_value_check_type(uint, UINT, 16);
    _nk_token_list_range_value_check_type(uint, UINT, 32);
    _nk_token_list_range_value_check_type(uint, UINT, 64);
    else if ( g_variant_is_of_type(var, G_VARIANT_TYPE_BYTE) )
        *value = g_variant_get_byte(var);
    else if ( g_variant_is_of_type(var, G_VARIANT_TYPE_BOOLEAN) )
        *value = g_variant_get_boolean(var) ? 1 : 0;
    else
    {
        g_set_error(error, NK_TOKEN_ERROR, NK_TOKEN_ERROR_WRONG_RANGE, "Invalid range value type: %s", g_variant_get_type_string(var));
        return FALSE;
    }

#undef _nk_token_list_range_value_check_type

    return TRUE;
}

static gboolean
_nk_token_list_parse_range_value(const gchar *s, const gchar *e, gdouble *value, GError **error)
{
    GError *_inner_error_ = NULL;
    GVariant *var;
    var = g_variant_parse(NULL, s, e, NULL, &_inner_error_);
    if ( var == NULL )
    {
        g_set_error(error, NK_TOKEN_ERROR, NK_TOKEN_ERROR_WRONG_RANGE, "Invalid range value: %s", _inner_error_->message);
        g_error_free(_inner_error_);
        return FALSE;
    }

    gboolean ret;
    ret = _nk_token_double_from_variant(var, value, error);

    g_variant_unref(var);

    return ret;
}

static gboolean
_nk_token_list_search_enum_tokens(NkTokenList *self, const gchar * const *tokens, guint64 size, guint64 *used_tokens, GError **error)
{
    gsize i;
    for ( i = 0 ; i < self->size ; ++i )
    {
        if ( self->tokens[i].name == NULL )
            continue;
        if ( ! nk_enum_parse(self->tokens[i].name, tokens, size, FALSE, FALSE, &self->tokens[i].value) )
        {
            g_set_error(error, NK_TOKEN_ERROR, NK_TOKEN_ERROR_UNKNOWN_TOKEN, "Unknown token: %s", self->tokens[i].name);
            return FALSE;
        }
        *used_tokens |= (1 << self->tokens[i].value);

        if ( self->tokens[i].fallback != NULL )
            _nk_token_list_search_enum_tokens(self->tokens[i].fallback, tokens, size, used_tokens, error);
        if ( self->tokens[i].substitute != NULL )
            _nk_token_list_search_enum_tokens(self->tokens[i].substitute, tokens, size, used_tokens, error);
        if ( self->tokens[i].replace != NULL )
        {
            NkTokenRegex *regex;
            for ( regex = self->tokens[i].replace ; regex->regex != NULL ; ++regex )
                _nk_token_list_search_enum_tokens(regex->replacement, tokens, size, used_tokens, error);
        }
    }
    return TRUE;
}

static NkTokenList *_nk_token_list_parse(gboolean owned, gchar *string, gunichar identifier, GError **error);
static NkTokenList *
_nk_token_list_parse_enum(gboolean owned, gchar *string, gunichar identifier, const gchar * const *tokens, guint64 size, guint64 *ret_used_tokens, GError **error)
{
    g_return_val_if_fail(string != NULL, NULL);

    NkTokenList *self;

    self = _nk_token_list_parse(owned, string, identifier, error);
    if ( self == NULL )
        return NULL;

    guint64 used_tokens = 0;
    if ( ! _nk_token_list_search_enum_tokens(self, tokens, size, &used_tokens, error) )
        goto fail;
    if ( ret_used_tokens != NULL )
        *ret_used_tokens = used_tokens;

    return self;

fail:
    nk_token_list_unref(self);
    return NULL;
}

static NkTokenList *
_nk_token_list_parse(gboolean owned, gchar *string, gunichar identifier, GError **error)
{
    g_return_val_if_fail(string != NULL, NULL);
    g_return_val_if_fail(error == NULL || *error == NULL, NULL);

    NkTokenList *self;

    self = g_new0(NkTokenList, 1);
    self->ref_count = 1;
    self->owned = owned;
    self->string = string;
    self->length = strlen(self->string);

    gboolean have_identifier = ( identifier != '\0' );
    if ( ! have_identifier )
        identifier = '{';

    gchar *w = string;
    while ( ( w = g_utf8_strchr(w, self->length - ( w - self->string ), identifier) ) != NULL )
    {
        gchar *b = w;

        if ( have_identifier )
        {
            w = g_utf8_next_char(w);
            if ( g_utf8_get_char(w) == identifier )
            {
                *w = '\0';
                if ( *string != '\0' )
                {
                    NkToken token = {
                        .string = string
                    };
                    ++self->size;
                    self->tokens = g_renew(NkToken, self->tokens, self->size);
                    self->tokens[self->size - 1] = token;
                }
                string = w = g_utf8_next_char(w);
                continue;
            }

            if ( g_utf8_get_char(w) != '{' )
                continue;
        }

        w = g_utf8_next_char(w);
        gchar *e;
        NkToken token = {
            .name = w
        };

        /* References are alpha/-/_ only */
        while ( g_unichar_isalpha(g_utf8_get_char(w)) || ( g_utf8_get_char(w) == '-' ) || ( g_utf8_get_char(w) == '_' ) )
            w = g_utf8_next_char(w);

        /* Empty name */
        if ( token.name == w )
            continue;

        e = _nk_token_strchr_escape(w, self->length - ( w - self->string ), '}', '{');
        if ( e == NULL )
            continue;
        gchar *next = g_utf8_next_char(e);

        if ( ( w != e ) && ( g_utf8_get_char(w) == '[' ) )
        {
            gchar *ss = w;
            w = g_utf8_next_char(w);
            const gchar *key = w;
            gint64 index = 0;
            if ( g_unichar_isalpha(g_utf8_get_char(w)) )
            {
                while ( g_unichar_isalpha(g_utf8_get_char(w)) || ( g_utf8_get_char(w) == '-' ) || ( g_utf8_get_char(w) == '_' ) )
                    w = g_utf8_next_char(w);
            }
            else if ( g_utf8_get_char(w) == '@' )
            {
                /* We consider the rest as a join token */
                while ( g_utf8_get_char(w) != ']' )
                    w = g_utf8_next_char(w);
            }
            else
            {
                gchar *ie;
                errno = 0;
                key = "";
                index = g_ascii_strtoll(w, &ie, 10);
                if ( ( errno != 0 ) || ( w == ie ) )
                {
                    g_set_error(error, NK_TOKEN_ERROR, NK_TOKEN_ERROR_WRONG_KEY, "Could not parse index value: %s", ss);
                    goto fail;
                }
                w = ie;
            }

            if ( g_utf8_get_char(w) != ']' )
            {
                g_set_error(error, NK_TOKEN_ERROR, NK_TOKEN_ERROR_WRONG_KEY, "Wrong key value: %s", ss);
                goto fail;
            }

            gchar *ie = w;
            w = g_utf8_next_char(w);
            *ss = '\0';
            *ie = '\0';
            token.key = key;
            token.index = index;
        }

        if ( w != e )
        switch ( g_utf8_get_char(w) )
        {
        case ':':
        {
            gchar *m = w;
            w = g_utf8_next_char(w);
            *e = '\0';

            switch ( g_utf8_get_char(w) )
            {
            case '-':
                token.fallback = _nk_token_list_parse(FALSE, g_utf8_next_char(w), identifier, error);
                if ( token.fallback == NULL )
                    goto fail;
            break;
            case '+':
                token.substitute = _nk_token_list_parse(FALSE, g_utf8_next_char(w), identifier, error);
                if ( token.substitute == NULL )
                    goto fail;
            break;
            case '!':
                token.no_data = TRUE;
                token.fallback = _nk_token_list_parse(FALSE, g_utf8_next_char(w), identifier, error);
                if ( token.fallback == NULL )
                    goto fail;
            break;
            case '[':
            {
                w = g_utf8_next_char(w);
                e = _nk_token_strchr_escape(w, e - w, ']', '[');
                if ( e == NULL )
                {
                    g_set_error(error, NK_TOKEN_ERROR, NK_TOKEN_ERROR_WRONG_RANGE, "Missing range close bracket: %s", w);
                    goto fail;
                }
                *e = '\0';

                gunichar sep = g_utf8_get_char(w);
                gchar *s;

                w = g_utf8_next_char(w);
                if ( ( s = g_utf8_strchr(w, e - w, sep) ) == NULL )
                {
                    g_set_error(error, NK_TOKEN_ERROR, NK_TOKEN_ERROR_WRONG_RANGE, "Missing range minimum value: %s", w);
                    goto fail;
                }
                if ( ! _nk_token_list_parse_range_value(w, s, &token.range.min, error) )
                    goto fail;

                w = g_utf8_next_char(s);
                if ( ( s = g_utf8_strchr(w, e - w, sep) ) == NULL )
                {
                    g_set_error(error, NK_TOKEN_ERROR, NK_TOKEN_ERROR_WRONG_RANGE, "Missing range maximum value: %s", w);
                    goto fail;
                }
                if ( ! _nk_token_list_parse_range_value(w, s, &token.range.max, error) )
                    goto fail;

                if ( token.range.min > token.range.max )
                {
                    g_set_error(error, NK_TOKEN_ERROR, NK_TOKEN_ERROR_WRONG_RANGE, "Range minimum value is bigger than the valueimum value: min=%lf > value=%lf", token.range.min, token.range.max);
                    goto fail;
                }

                do
                {
                    w = g_utf8_next_char(s);
                    *s = '\0';
                    token.range.values = g_renew(gchar *, token.range.values, ++token.range.length);
                    token.range.values[token.range.length - 1] = w;
                } while ( ( s = g_utf8_strchr(w, e - w, sep) ) != NULL );
            }
            break;
            case '{':
            {
                w = g_utf8_next_char(w);
                e = _nk_token_strchr_escape(w, e - w, '}', '{');
                if ( e == NULL )
                {
                    g_set_error(error, NK_TOKEN_ERROR, NK_TOKEN_ERROR_WRONG_RANGE, "Missing switch close bracket: %s", w);
                    goto fail;
                }
                *e = '\0';

                gunichar sep = g_utf8_get_char(w);
                gchar *s;

                w = g_utf8_next_char(w);
                if ( ( s = g_utf8_strchr(w, e - w, sep) ) == NULL )
                {
                    g_set_error(error, NK_TOKEN_ERROR, NK_TOKEN_ERROR_WRONG_SWITCH, "Missing switch false value: %s", w);
                    goto fail;
                }
                token.switch_.true_ = w;
                w = g_utf8_next_char(s);
                *s = '\0';
                token.switch_.false_ = w;
            }
            break;
            default:
                /* Just fail on malformed string */
                *g_utf8_next_char(w) = '\0';
                g_set_error(error, NK_TOKEN_ERROR, NK_TOKEN_ERROR_UNKNOWN_MODIFIER, "Wrong modifier value: %s", w);
                goto fail;
            }
            *m = '\0';
        }
        break;
        case '(':
        {
            gchar *m = w;
            w = g_utf8_next_char(w);
            *e = *m = '\0';
            e = _nk_token_strchr_escape(w, e - w, ')', '(');
            if ( e == NULL )
            {
                g_set_error(error, NK_TOKEN_ERROR, NK_TOKEN_ERROR_WRONG_PRETIFFY, "Missing prettify close paren: %s", w);
                goto fail;
            }
            *e = '\0';
            token.prettify.type = g_utf8_get_char(w);
            token.prettify.width = 0;
            token.prettify.precision = -1;
            w = g_utf8_next_char(w);
            switch ( token.prettify.type )
            {
            case NK_TOKEN_PRETTIFY_FLOAT:
            case NK_TOKEN_PRETTIFY_PREFIXES_SI:
            case NK_TOKEN_PRETTIFY_PREFIXES_BINARY:
            break;
            case NK_TOKEN_PRETTIFY_TIME:
                if ( w != e )
                    token.prettify.time_format = w;
                else
                    token.prettify.time_format = "%c";
                w = e;
                goto end_prettify;
            case NK_TOKEN_PRETTIFY_DURATION:
                token.prettify.duration_format = _nk_token_list_parse_enum(( w == e ), ( w == e ) ? g_strdup(NK_TOKEN_PRETTIFY_DURATION_DEFAULT) : w, '%', _nk_token_list_prettify_duration_tokens, G_N_ELEMENTS(_nk_token_list_prettify_duration_tokens), NULL, error);
                if ( token.prettify.duration_format == NULL )
                    goto fail;
                w = e;
                goto end_prettify;
            default:
                /* Just fail on malformed string */
                *w = '\0';
                g_set_error(error, NK_TOKEN_ERROR, NK_TOKEN_ERROR_WRONG_PRETIFFY, "Wrong prettify identifier: %s", m);
                goto fail;
            }

            if ( g_utf8_get_char(w) == '0' )
            {
                g_snprintf(token.prettify.format, sizeof(token.prettify.format), "%%0*.*lf%%s");
                w = g_utf8_next_char(w);
            }
            else
                g_snprintf(token.prettify.format, sizeof(token.prettify.format), "%%*.*lf%%s");
            if ( w == e )
                break;
            if ( g_unichar_isdigit(g_utf8_get_char(w)) )
            {
                gchar *ie;
                errno = 0;
                token.prettify.width = g_ascii_strtoll(w, &ie, 10);
                if ( ( errno != 0 ) || ( w == ie ) )
                {
                    g_set_error(error, NK_TOKEN_ERROR, NK_TOKEN_ERROR_WRONG_PRETIFFY, "Could not parse pretiffy width: %s", w);
                    goto fail;
                }
                w = ie;
            }

            if ( g_utf8_get_char(w) == '.' )
            {
                w = g_utf8_next_char(w);
                gchar *ie;
                errno = 0;
                token.prettify.precision = g_ascii_strtoll(w, &ie, 10);
                if ( ( errno != 0 ) || ( w == ie ) )
                {
                    g_set_error(error, NK_TOKEN_ERROR, NK_TOKEN_ERROR_WRONG_PRETIFFY, "Could not parse pretiffy precision: %s", w);
                    goto fail;
                }
                w = ie;
            }

        end_prettify:
            if ( w != e )
            {
                g_set_error(error, NK_TOKEN_ERROR, NK_TOKEN_ERROR_WRONG_PRETIFFY, "Unexpected leftovers in prettify: %s", w);
                goto fail;
            }
        }
        break;
        case '/':
        {
            gchar *m = w;
            *e = '\0';

            gsize c = 0;
            do
            {
                ++c;
                *m = '\0';
            } while ( ( m = _nk_token_strchr_escape(m, e - m, '/', '\0') ) != NULL );
            c = ( c + 1 ) / 2 + 1;

            token.replace = g_new(NkTokenRegex, c);
            c = 0;
            do
            {
                GError *_inner_error_ = NULL;
                token.replace[c].regex = g_regex_new(++w, G_REGEX_OPTIMIZE, 0, &_inner_error_);
                if ( token.replace[c].regex == NULL )
                {
                    g_set_error(error, NK_TOKEN_ERROR, NK_TOKEN_ERROR_REGEX, "Wrong regex: %s", _inner_error_->message);
                    g_clear_error(&_inner_error_);
                    goto fail;
                }

                w = w + strlen(w) + 1;
                token.replace[c].replacement = _nk_token_list_parse(FALSE, ( w > e ) ? "" : w, identifier, error);
                if ( token.replace[c].replacement == NULL )
                    goto fail;
                w += strlen(w);
                ++c;
            } while ( w < e );
            token.replace[c].regex = NULL;
        }
        break;
        default:
            continue;
        }

        *e = *b = '\0';

        ++self->size;
        if ( *string != '\0' )
            ++self->size;
        self->tokens = g_renew(NkToken, self->tokens, self->size);
        if ( *string != '\0' )
        {
            NkToken stoken = {
                .string = string
            };
            self->tokens[self->size - 2] = stoken;
        }
        self->tokens[self->size - 1] = token;

        string = w = next;
    }
    self->tokens = g_renew(NkToken, self->tokens, ++self->size);
    NkToken token = {
        .string = string
    };
    self->tokens[self->size - 1] = token;

    return self;

fail:
    nk_token_list_unref(self);
    return NULL;
}

NkTokenList *
nk_token_list_parse(gchar *string, gunichar identifier, GError **error)
{
    return _nk_token_list_parse(TRUE, string, identifier, error);
}

NkTokenList *
nk_token_list_parse_enum(gchar *string, gunichar identifier, const gchar * const *tokens, guint64 size, guint64 *ret_used_tokens, GError **error)
{
    return _nk_token_list_parse_enum(TRUE, string, identifier, tokens, size, ret_used_tokens, error);
}

NkTokenList *
nk_token_list_ref(NkTokenList *self)
{
    g_return_val_if_fail(self != NULL, NULL);
    ++self->ref_count;
    return self;
}

static void
_nk_token_list_free(NkTokenList *self)
{
    if ( self->owned )
        g_free(self->string);

    gsize i;
    for ( i = 0 ; i < self->size ; ++i )
    {
        if ( self->tokens[i].substitute != NULL)
            _nk_token_list_free(self->tokens[i].substitute);
        else if ( self->tokens[i].range.length > 0 )
            g_free(self->tokens[i].range.values);
        else if ( self->tokens[i].replace != NULL )
        {
            NkTokenRegex *regex;
            for ( regex = self->tokens[i].replace ; regex->regex != NULL ; ++regex )
            {
                g_regex_unref(regex->regex);
                _nk_token_list_free(regex->replacement);
            }
            g_free(self->tokens[i].replace);
        }
        else if ( self->tokens[i].fallback != NULL )
            _nk_token_list_free(self->tokens[i].fallback);
    }

    g_free(self->tokens);

    g_free(self);
}

void
nk_token_list_unref(NkTokenList *self)
{
    g_return_if_fail(self != NULL);
    if ( --self->ref_count > 0 )
        return;

    _nk_token_list_free(self);
}

static GVariant *
_nk_token_list_search_data(GVariant *source, const gchar *key, gint64 index, const gchar **joiner)
{
    if ( source == NULL )
        return NULL;

    GVariant *data = source;
    while ( g_variant_is_of_type(data, G_VARIANT_TYPE_VARIANT) )
        data = g_variant_get_variant(data);

    if ( g_variant_is_of_type(data, G_VARIANT_TYPE_DICTIONARY) )
    {
        if ( ( key == NULL ) || ( g_utf8_get_char(key) == '\0' ) )
            data = NULL;
        else
            data = g_variant_lookup_value(data, key, NULL);
    }
    else if ( g_variant_is_of_type(data, G_VARIANT_TYPE_ARRAY) )
    {
        gsize length;
        length = g_variant_n_children(data);

        if ( length == 0 )
            data = NULL;
        else if ( key != NULL )
        switch ( g_utf8_get_char(key) )
        {
        case '\0':
        {
            gsize i = ABS(index);
            if ( ( index < 0 ) && ( i <= length ) )
                data = g_variant_get_child_value(data, length - i);
            else if ( ( index >= 0 ) && ( i < length ) )
                data = g_variant_get_child_value(data, i);
            else
                data = NULL;
        }
        break;
        case '@':
        {
            const gchar *s = g_utf8_next_char(key);
            if ( g_utf8_get_char(s) != '\0')
                *joiner = s;
        }
        break;
        default:
            data = NULL;
        }
    }

    if ( data != NULL )
    {
        while ( g_variant_is_of_type(data, G_VARIANT_TYPE_VARIANT) )
            data = g_variant_get_variant(data);

        g_variant_ref(data);
    }
    g_variant_unref(source);
    return data;
}

static gboolean
_nk_token_list_check_data(GVariant *data, const NkToken *token)
{
    if ( data == NULL )
        return FALSE;

    if ( g_variant_is_of_type(data, G_VARIANT_TYPE_BOOLEAN) )
    {
        /* We want a boolean data to still be replaced if it’s not checked against */
        if ( ! g_variant_get_boolean(data) )
            return ( ( token->fallback == NULL ) && ( token->substitute == NULL ) );
    }

    return TRUE;
}

static void _nk_token_list_replace(GString *string, const NkTokenList *self, NkTokenListReplaceCallback callback, gpointer user_data);

static void
_nk_token_list_append_range(GString *string, GVariant *data, NkTokenRange *range)
{
    gdouble value;
    if ( ! _nk_token_double_from_variant(data, &value, NULL) )
        return;

    gdouble v, r;
    gsize i;

    if ( value >= range->max )
        i = range->length - 1;
    else if ( value < range->min )
        i = 0;
    else
    {
        v = value - range->min;
        r = range->max - range->min;
        i = (gsize) ( (gdouble) ( range->length ) * ( v / r ) );
    }
    g_string_append(string, range->values[i]);
}

static void
_nk_token_list_append_switch(GString *string, GVariant *data, NkTokenSwitch *switch_)
{
    if ( ! g_variant_is_of_type(data, G_VARIANT_TYPE_BOOLEAN) )
        return;

    g_string_append(string, g_variant_get_boolean(data) ? switch_->true_ : switch_->false_);
}

static GVariant *
_nk_token_list_prettify_duration_callback(G_GNUC_UNUSED const gchar *token, guint64 value, gpointer user_data)
{
    NkTokenListPrettifyDurationData *data = user_data;
    switch ( (NkTokenListPrettifyDurationToken) value )
    {
    case NK_TOKEN_PRETTIFY_DURATION_TOKEN_WEEKS:
        if ( data->w == 0 )
            return NULL;
        return g_variant_new_uint64(data->w);
    case NK_TOKEN_PRETTIFY_DURATION_TOKEN_DAYS:
        if ( data->d == 0 )
            return NULL;
        return g_variant_new_uint64(data->d);
    case NK_TOKEN_PRETTIFY_DURATION_TOKEN_HOURS:
        if ( data->h == 0 )
            return NULL;
        return g_variant_new_uint64(data->h);
    case NK_TOKEN_PRETTIFY_DURATION_TOKEN_MINUTES:
        if ( data->m == 0 )
            return NULL;
        return g_variant_new_uint64(data->m);
    case NK_TOKEN_PRETTIFY_DURATION_TOKEN_SECONDS:
        if ( data->s == 0 )
            return NULL;
        return g_variant_new_uint64(data->s);
    case NK_TOKEN_PRETTIFY_DURATION_TOKEN_MILLISECONDS:
        if ( data->ms == 0 )
            return NULL;
        return g_variant_new_uint16(data->ms);
    case NK_TOKEN_PRETTIFY_DURATION_TOKEN_MICROSECONDS:
        if ( data->us == 0 )
            return NULL;
        return g_variant_new_uint16(data->us);
    case NK_TOKEN_PRETTIFY_DURATION_TOKEN_NANOSECONDS:
        if ( data->ns == 0 )
            return NULL;
        return g_variant_new_uint16(data->ns);
    }
    return NULL;
}

static const gchar *_nk_token_prefixes_si_big[] = {
    "", "k", "M", "G", "T", "P", "E"
};
static const gchar *_nk_token_prefixes_si_small[] = {
    "", "m", "µ", "n", "p", "f", "a"
};
static const gchar *_nk_token_prefixes_binary[] = {
    "", "Ki", "Mi", "Gi", "Ti"
};

static void
_nk_token_list_append_prettify(GString *string, GVariant *data, NkTokenPrettify *prettify)
{
    gdouble value;
    if ( ! _nk_token_double_from_variant(data, &value, NULL) )
        return;

    switch ( prettify->type )
    {
    case NK_TOKEN_PRETTIFY_NONE:
        g_return_if_reached();
    case NK_TOKEN_PRETTIFY_FLOAT:
    {
        gint precision = prettify->precision;
        if ( value == (gdouble) ( (gint64) value ) )
            precision = MAX(0, precision);
        g_string_append_printf(string, prettify->format, prettify->width, precision, value, "");
    }
    break;
    case NK_TOKEN_PRETTIFY_PREFIXES_SI:
    {
        const gchar **prefix;
        if ( value == 0 )
            prefix = _nk_token_prefixes_si_small; /* Empty string */
        else if ( ( value > -1 ) && ( value < 1 ) )
        {
            for ( prefix = _nk_token_prefixes_si_small ; ( value > -1 ) && ( value < 1 ) && ( prefix < ( _nk_token_prefixes_si_small + G_N_ELEMENTS(_nk_token_prefixes_si_small) ) ) ; ++prefix )
                value *= 1000;
        }
        else for ( prefix = _nk_token_prefixes_si_big ; ( value >= 1000 ) && ( prefix < ( _nk_token_prefixes_si_big + G_N_ELEMENTS(_nk_token_prefixes_si_big) ) ) ; ++prefix )
            value /= 1000;
        gint precision = prettify->precision;
        if ( value == (gdouble) ( (gint64) value ) )
            precision = MAX(0, precision);
        g_string_append_printf(string, prettify->format, prettify->width, precision, value, *prefix);
    }
    break;
    case NK_TOKEN_PRETTIFY_PREFIXES_BINARY:
    {
        const gchar **prefix;
        for ( prefix = _nk_token_prefixes_binary ; ( value >= 1024 ) && ( prefix < ( _nk_token_prefixes_binary + G_N_ELEMENTS(_nk_token_prefixes_binary) ) ) ; ++prefix )
            value /= 1024;
        gint precision = prettify->precision;
        if ( value == (gdouble) ( (gint64) value ) )
            precision = MAX(0, precision);
        g_string_append_printf(string, prettify->format, prettify->width, precision, value, *prefix);
    }
    break;
    case NK_TOKEN_PRETTIFY_TIME:
    {
        GDateTime *time;
        gchar *tmp = NULL;
        time = g_date_time_new_from_unix_local(value);
        if ( time != NULL )
        {
            tmp = g_date_time_format(time, prettify->time_format);
            g_date_time_unref(time);
        }
        if ( tmp != NULL )
            g_string_append(string, tmp);
        g_free(tmp);
    }
    break;
    case NK_TOKEN_PRETTIFY_DURATION:
    {
        NkTokenListPrettifyDurationData data = { .w = 0 };
        guint64 s = (guint64) value;
        value -= s;

        if ( s > 604800 )
        {
            data.w = s / 604800;
            s %= 604800;
        }

        if ( s > 86400 )
        {
            data.d = s / 86400;
            s %= 86400;
        }

        if ( s > 3600 )
        {
            data.h = s / 3600;
            s %= 3600;
        }

        if ( s > 60 )
        {
            data.m = s / 60;
            s %= 60;
        }

        data.s = s;

        if ( value >= 0.001 )
        {
            data.ms = (guint16) ( value * 1000 );
            value -= ( data.ms / 1000. );
        }

        if ( value >= 0.000001 )
        {
            data.us = (guint16) ( value * 1000000 );
            value -= ( data.us / 1000000. );
        }

        if ( value >= 0.000000001 )
        {
            data.ns = (guint16) ( value * 1000000000 );
            value -= ( data.ns / 1000000000. );
        }

        _nk_token_list_replace(string, prettify->duration_format, _nk_token_list_prettify_duration_callback, &data);
    }
    break;
    }
}

static void
_nk_token_list_append_data(GString *string, GVariant *data, const gchar *joiner)
{
    if ( g_variant_is_of_type(data, G_VARIANT_TYPE_ARRAY) )
    {
        gsize length;
        length = g_variant_n_children(data);

        gsize jl = strlen(joiner);

        gsize i;
        for ( i = 0 ; i < length ; ++i )
        {
            _nk_token_list_append_data(string, g_variant_get_child_value(data, i), joiner);
            g_string_append(string, joiner);
        }
        g_string_truncate(string, string->len - jl);
    }
    else if ( g_variant_is_of_type(data, G_VARIANT_TYPE_STRING) )
        g_string_append(string, g_variant_get_string(data, NULL));
    else if ( g_variant_is_of_type(data, G_VARIANT_TYPE_BOOLEAN) )
        g_string_append(string, g_variant_get_boolean(data) ? "true" : "false");

#define _nk_token_list_check_type_with_format(l, U, GFormat) \
    else if ( g_variant_is_of_type(data, G_VARIANT_TYPE_##U) ) \
            g_string_append_printf(string, "%" GFormat, g_variant_get_##l(data))
#define _nk_token_list_check_type(l, U) _nk_token_list_check_type_with_format(l, U, G_G##U##_FORMAT)

    _nk_token_list_check_type(int16, INT16);
    _nk_token_list_check_type(int32, INT32);
    _nk_token_list_check_type(int64, INT64);
    _nk_token_list_check_type_with_format(byte, BYTE, "hhu");
    _nk_token_list_check_type(uint16, UINT16);
    _nk_token_list_check_type(uint32, UINT32);
    _nk_token_list_check_type(uint64, UINT64);
    _nk_token_list_check_type_with_format(double, DOUBLE, "lf");

#undef _nk_token_list_check_type
#undef _nk_token_list_check_type_with_format

    else
        g_variant_print_string(data, string, FALSE);
}

static void
_nk_token_list_replace(GString *string, const NkTokenList *self, NkTokenListReplaceCallback callback, gpointer user_data)
{
    gsize i;
    for ( i = 0 ; i < self->size ; ++i )
    {
        if ( self->tokens[i].string != NULL )
        {
            g_string_append(string, self->tokens[i].string);
            continue;
        }

        GVariant *data;
        const gchar *joiner = ", ";
        data = callback(self->tokens[i].name, self->tokens[i].value, user_data);
        data = _nk_token_list_search_data(data, self->tokens[i].key, self->tokens[i].index, &joiner);
        if ( _nk_token_list_check_data(data, &self->tokens[i]) )
        {
            if ( self->tokens[i].substitute != NULL)
                _nk_token_list_replace(string, self->tokens[i].substitute, callback, user_data);
            else if ( self->tokens[i].range.length > 0 )
                _nk_token_list_append_range(string, data, &self->tokens[i].range);
            else if ( self->tokens[i].switch_.true_ != NULL )
                _nk_token_list_append_switch(string, data, &self->tokens[i].switch_);
            else if ( self->tokens[i].prettify.type != NK_TOKEN_PRETTIFY_NONE )
                _nk_token_list_append_prettify(string, data, &self->tokens[i].prettify);
            else if ( self->tokens[i].replace != NULL )
            {
                NkTokenRegex *regex;
                GString *tmp;
                gchar *from;
                gchar *to = NULL;

                tmp = g_string_new("");
                _nk_token_list_append_data(tmp, data, joiner);
                from = g_string_free(tmp, FALSE);
                for ( regex = self->tokens[i].replace ; regex->regex != NULL ; ++regex )
                {
                    gchar *replacement;
                    replacement = nk_token_list_replace(regex->replacement, callback, user_data);
                    to = g_regex_replace(regex->regex, from, -1, 0, replacement, 0, NULL);
                    g_free(replacement);
                    g_free(from);
                    if ( to == NULL )
                        break;
                    from = to;
                }
                if ( to != NULL )
                    g_string_append(string, to);
                g_free(to);
            }
            else if ( ! self->tokens[i].no_data )
                _nk_token_list_append_data(string, data, joiner);
            g_variant_unref(data);
        }
        else if ( self->tokens[i].fallback != NULL )
            _nk_token_list_replace(string, self->tokens[i].fallback, callback, user_data);
    }
}

gchar *
nk_token_list_replace(const NkTokenList *self, NkTokenListReplaceCallback callback, gpointer user_data)
{
    g_return_val_if_fail(self != NULL, NULL);
    g_return_val_if_fail(callback != NULL, NULL);

    GString *string;
    string = g_string_sized_new(self->length);

    _nk_token_list_replace(string, self, callback, user_data);

    return g_string_free(string, FALSE);
}
