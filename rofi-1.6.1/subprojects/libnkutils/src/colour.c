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

#ifdef G_LOG_DOMAIN
#undef G_LOG_DOMAIN
#endif /* G_LOG_DOMAIN */
#define G_LOG_DOMAIN "libnkutils-colour"

#include <string.h>

#include <glib.h>
#include <glib/gprintf.h>

#include "nkutils-colour.h"

#define DMODULO(d, m) ( (d) - (guint64) (d) + ( (guint64) (d) % (m) ) )

#define RGB_TO_COLOUR(r, g, b) { .red = (gdouble) (r) / 255., .green = (gdouble) (g) / 255., .blue = (gdouble) (b) / 255., .alpha = 1. }

static GScanner *_nk_colour_scanner = NULL;

typedef enum {
    NK_COLOUR_SCOPE_BASE,
    NK_COLOUR_SCOPE_ANGLES,
} NkColourScope;

typedef enum {
    NK_COLOUR_SYMBOL_BASE_RGB,
    NK_COLOUR_SYMBOL_BASE_RGBA,
    NK_COLOUR_SYMBOL_BASE_HSL,
    NK_COLOUR_SYMBOL_BASE_HSLA,
    NK_COLOUR_SYMBOL_BASE_HWB,
} NkColourSymbolBase;

typedef enum {
    NK_COLOUR_ANGLE_DEG,
    NK_COLOUR_ANGLE_GRAD,
    NK_COLOUR_ANGLE_RAD,
    NK_COLOUR_ANGLE_TURN,
} NkColourAngle;

static const gchar * const _nk_colour_scanner_symbols_base[] = {
    [NK_COLOUR_SYMBOL_BASE_RGB] = "rgb",
    [NK_COLOUR_SYMBOL_BASE_RGBA] = "rgba",
    [NK_COLOUR_SYMBOL_BASE_HSL] = "hsl",
    [NK_COLOUR_SYMBOL_BASE_HSLA] = "hsla",
    [NK_COLOUR_SYMBOL_BASE_HWB] = "hwb",
};

static const gchar * const _nk_colour_scanner_symbols_angle_units[] = {
    [NK_COLOUR_ANGLE_DEG] = "deg",
    [NK_COLOUR_ANGLE_GRAD] = "grad",
    [NK_COLOUR_ANGLE_RAD] = "rad",
    [NK_COLOUR_ANGLE_TURN] = "turn",
};

static const struct {
    const gchar *name;
    NkColour value;
} _nk_colour_scanner_named_colours[] = {
    { "transparent", { .red = 0., .green = 0., .blue = 0., .alpha = 0. } },
    { "aliceblue", RGB_TO_COLOUR(240, 248, 255) },
    { "antiquewhite", RGB_TO_COLOUR(250, 235, 215) },
    { "aqua", RGB_TO_COLOUR(0, 255, 255) },
    { "aquamarine", RGB_TO_COLOUR(127, 255, 212) },
    { "azure", RGB_TO_COLOUR(240, 255, 255) },
    { "beige", RGB_TO_COLOUR(245, 245, 220) },
    { "bisque", RGB_TO_COLOUR(255, 228, 196) },
    { "black", RGB_TO_COLOUR(0, 0, 0) },
    { "blanchedalmond", RGB_TO_COLOUR(255, 235, 205) },
    { "blue", RGB_TO_COLOUR(0, 0, 255) },
    { "blueviolet", RGB_TO_COLOUR(138, 43, 226) },
    { "brown", RGB_TO_COLOUR(165, 42, 42) },
    { "burlywood", RGB_TO_COLOUR(222, 184, 135) },
    { "cadetblue", RGB_TO_COLOUR(95, 158, 160) },
    { "chartreuse", RGB_TO_COLOUR(127, 255, 0) },
    { "chocolate", RGB_TO_COLOUR(210, 105, 30) },
    { "coral", RGB_TO_COLOUR(255, 127, 80) },
    { "cornflowerblue", RGB_TO_COLOUR(100, 149, 237) },
    { "cornsilk", RGB_TO_COLOUR(255, 248, 220) },
    { "crimson", RGB_TO_COLOUR(220, 20, 60) },
    { "cyan", RGB_TO_COLOUR(0, 255, 255) },
    { "darkblue", RGB_TO_COLOUR(0, 0, 139) },
    { "darkcyan", RGB_TO_COLOUR(0, 139, 139) },
    { "darkgoldenrod", RGB_TO_COLOUR(184, 134, 11) },
    { "darkgray", RGB_TO_COLOUR(169, 169, 169) },
    { "darkgreen", RGB_TO_COLOUR(0, 100, 0) },
    { "darkgrey", RGB_TO_COLOUR(169, 169, 169) },
    { "darkkhaki", RGB_TO_COLOUR(189, 183, 107) },
    { "darkmagenta", RGB_TO_COLOUR(139, 0, 139) },
    { "darkolivegreen", RGB_TO_COLOUR(85, 107, 47) },
    { "darkorange", RGB_TO_COLOUR(255, 140, 0) },
    { "darkorchid", RGB_TO_COLOUR(153, 50, 204) },
    { "darkred", RGB_TO_COLOUR(139, 0, 0) },
    { "darksalmon", RGB_TO_COLOUR(233, 150, 122) },
    { "darkseagreen", RGB_TO_COLOUR(143, 188, 143) },
    { "darkslateblue", RGB_TO_COLOUR(72, 61, 139) },
    { "darkslategray", RGB_TO_COLOUR(47, 79, 79) },
    { "darkslategrey", RGB_TO_COLOUR(47, 79, 79) },
    { "darkturquoise", RGB_TO_COLOUR(0, 206, 209) },
    { "darkviolet", RGB_TO_COLOUR(148, 0, 211) },
    { "deeppink", RGB_TO_COLOUR(255, 20, 147) },
    { "deepskyblue", RGB_TO_COLOUR(0, 191, 255) },
    { "dimgray", RGB_TO_COLOUR(105, 105, 105) },
    { "dimgrey", RGB_TO_COLOUR(105, 105, 105) },
    { "dodgerblue", RGB_TO_COLOUR(30, 144, 255) },
    { "firebrick", RGB_TO_COLOUR(178, 34, 34) },
    { "floralwhite", RGB_TO_COLOUR(255, 250, 240) },
    { "forestgreen", RGB_TO_COLOUR(34, 139, 34) },
    { "fuchsia", RGB_TO_COLOUR(255, 0, 255) },
    { "gainsboro", RGB_TO_COLOUR(220, 220, 220) },
    { "ghostwhite", RGB_TO_COLOUR(248, 248, 255) },
    { "gold", RGB_TO_COLOUR(255, 215, 0) },
    { "goldenrod", RGB_TO_COLOUR(218, 165, 32) },
    { "gray", RGB_TO_COLOUR(128, 128, 128) },
    { "green", RGB_TO_COLOUR(0, 128, 0) },
    { "greenyellow", RGB_TO_COLOUR(173, 255, 47) },
    { "grey", RGB_TO_COLOUR(128, 128, 128) },
    { "honeydew", RGB_TO_COLOUR(240, 255, 240) },
    { "hotpink", RGB_TO_COLOUR(255, 105, 180) },
    { "indianred", RGB_TO_COLOUR(205, 92, 92) },
    { "indigo", RGB_TO_COLOUR(75, 0, 130) },
    { "ivory", RGB_TO_COLOUR(255, 255, 240) },
    { "khaki", RGB_TO_COLOUR(240, 230, 140) },
    { "lavender", RGB_TO_COLOUR(230, 230, 250) },
    { "lavenderblush", RGB_TO_COLOUR(255, 240, 245) },
    { "lawngreen", RGB_TO_COLOUR(124, 252, 0) },
    { "lemonchiffon", RGB_TO_COLOUR(255, 250, 205) },
    { "lightblue", RGB_TO_COLOUR(173, 216, 230) },
    { "lightcoral", RGB_TO_COLOUR(240, 128, 128) },
    { "lightcyan", RGB_TO_COLOUR(224, 255, 255) },
    { "lightgoldenrodyellow", RGB_TO_COLOUR(250, 250, 210) },
    { "lightgray", RGB_TO_COLOUR(211, 211, 211) },
    { "lightgreen", RGB_TO_COLOUR(144, 238, 144) },
    { "lightgrey", RGB_TO_COLOUR(211, 211, 211) },
    { "lightpink", RGB_TO_COLOUR(255, 182, 193) },
    { "lightsalmon", RGB_TO_COLOUR(255, 160, 122) },
    { "lightseagreen", RGB_TO_COLOUR(32, 178, 170) },
    { "lightskyblue", RGB_TO_COLOUR(135, 206, 250) },
    { "lightslategray", RGB_TO_COLOUR(119, 136, 153) },
    { "lightslategrey", RGB_TO_COLOUR(119, 136, 153) },
    { "lightsteelblue", RGB_TO_COLOUR(176, 196, 222) },
    { "lightyellow", RGB_TO_COLOUR(255, 255, 224) },
    { "lime", RGB_TO_COLOUR(0, 255, 0) },
    { "limegreen", RGB_TO_COLOUR(50, 205, 50) },
    { "linen", RGB_TO_COLOUR(250, 240, 230) },
    { "magenta", RGB_TO_COLOUR(255, 0, 255) },
    { "maroon", RGB_TO_COLOUR(128, 0, 0) },
    { "mediumaquamarine", RGB_TO_COLOUR(102, 205, 170) },
    { "mediumblue", RGB_TO_COLOUR(0, 0, 205) },
    { "mediumorchid", RGB_TO_COLOUR(186, 85, 211) },
    { "mediumpurple", RGB_TO_COLOUR(147, 112, 219) },
    { "mediumseagreen", RGB_TO_COLOUR(60, 179, 113) },
    { "mediumslateblue", RGB_TO_COLOUR(123, 104, 238) },
    { "mediumspringgreen", RGB_TO_COLOUR(0, 250, 154) },
    { "mediumturquoise", RGB_TO_COLOUR(72, 209, 204) },
    { "mediumvioletred", RGB_TO_COLOUR(199, 21, 133) },
    { "midnightblue", RGB_TO_COLOUR(25, 25, 112) },
    { "mintcream", RGB_TO_COLOUR(245, 255, 250) },
    { "mistyrose", RGB_TO_COLOUR(255, 228, 225) },
    { "moccasin", RGB_TO_COLOUR(255, 228, 181) },
    { "navajowhite", RGB_TO_COLOUR(255, 222, 173) },
    { "navy", RGB_TO_COLOUR(0, 0, 128) },
    { "oldlace", RGB_TO_COLOUR(253, 245, 230) },
    { "olive", RGB_TO_COLOUR(128, 128, 0) },
    { "olivedrab", RGB_TO_COLOUR(107, 142, 35) },
    { "orange", RGB_TO_COLOUR(255, 165, 0) },
    { "orangered", RGB_TO_COLOUR(255, 69, 0) },
    { "orchid", RGB_TO_COLOUR(218, 112, 214) },
    { "palegoldenrod", RGB_TO_COLOUR(238, 232, 170) },
    { "palegreen", RGB_TO_COLOUR(152, 251, 152) },
    { "paleturquoise", RGB_TO_COLOUR(175, 238, 238) },
    { "palevioletred", RGB_TO_COLOUR(219, 112, 147) },
    { "papayawhip", RGB_TO_COLOUR(255, 239, 213) },
    { "peachpuff", RGB_TO_COLOUR(255, 218, 185) },
    { "peru", RGB_TO_COLOUR(205, 133, 63) },
    { "pink", RGB_TO_COLOUR(255, 192, 203) },
    { "plum", RGB_TO_COLOUR(221, 160, 221) },
    { "powderblue", RGB_TO_COLOUR(176, 224, 230) },
    { "purple", RGB_TO_COLOUR(128, 0, 128) },
    { "rebeccapurple", RGB_TO_COLOUR(102, 51, 153) },
    { "red", RGB_TO_COLOUR(255, 0, 0) },
    { "rosybrown", RGB_TO_COLOUR(188, 143, 143) },
    { "royalblue", RGB_TO_COLOUR(65, 105, 225) },
    { "saddlebrown", RGB_TO_COLOUR(139, 69, 19) },
    { "salmon", RGB_TO_COLOUR(250, 128, 114) },
    { "sandybrown", RGB_TO_COLOUR(244, 164, 96) },
    { "seagreen", RGB_TO_COLOUR(46, 139, 87) },
    { "seashell", RGB_TO_COLOUR(255, 245, 238) },
    { "sienna", RGB_TO_COLOUR(160, 82, 45) },
    { "silver", RGB_TO_COLOUR(192, 192, 192) },
    { "skyblue", RGB_TO_COLOUR(135, 206, 235) },
    { "slateblue", RGB_TO_COLOUR(106, 90, 205) },
    { "slategray", RGB_TO_COLOUR(112, 128, 144) },
    { "slategrey", RGB_TO_COLOUR(112, 128, 144) },
    { "snow", RGB_TO_COLOUR(255, 250, 250) },
    { "springgreen", RGB_TO_COLOUR(0, 255, 127) },
    { "steelblue", RGB_TO_COLOUR(70, 130, 180) },
    { "tan", RGB_TO_COLOUR(210, 180, 140) },
    { "teal", RGB_TO_COLOUR(0, 128, 128) },
    { "thistle", RGB_TO_COLOUR(216, 191, 216) },
    { "tomato", RGB_TO_COLOUR(255, 99, 71) },
    { "turquoise", RGB_TO_COLOUR(64, 224, 208) },
    { "violet", RGB_TO_COLOUR(238, 130, 238) },
    { "wheat", RGB_TO_COLOUR(245, 222, 179) },
    { "white", RGB_TO_COLOUR(255, 255, 255) },
    { "whitesmoke", RGB_TO_COLOUR(245, 245, 245) },
    { "yellow", RGB_TO_COLOUR(255, 255, 0) },
    { "yellowgreen", RGB_TO_COLOUR(154, 205, 50) },
};

#define IS_BETWEEN(x, low, high) ( ( (x) >= (low) ) && ( (x) <= (high) ) )

static inline gboolean
_nk_colour_parse_hex(gdouble *r, gchar c1, gchar c2)
{
    gchar s[3] = { c1, c2, '\0' }, *e;
    guint64 v;

    v = g_ascii_strtoull(s, &e, 16);
    if ( s == e )
        return FALSE;
    if ( v > 255 )
        return FALSE;

    *r = (gdouble) v / 255.;
    return TRUE;
}

static inline gboolean
_nk_colour_parse_alpha_value(gdouble *r, gboolean expected)
{
    if ( expected != ( g_scanner_peek_next_token(_nk_colour_scanner) == G_TOKEN_COMMA ) )
        return FALSE;
    if ( ! expected )
        return TRUE;
    g_scanner_get_next_token(_nk_colour_scanner);

    if ( g_scanner_get_next_token(_nk_colour_scanner) != G_TOKEN_FLOAT )
        return FALSE;

    gdouble v = _nk_colour_scanner->value.v_float;
    if ( g_scanner_peek_next_token(_nk_colour_scanner) == '%' )
    {
        g_scanner_get_next_token(_nk_colour_scanner);

        if ( ! IS_BETWEEN(v, 0., 100.) )
            return FALSE;

        *r = v / 100.;
    }
    else
    {
        if ( ! IS_BETWEEN(v, 0., 1.) )
            return FALSE;

        *r = v;
    }

    return TRUE;
}

static inline gboolean
_nk_colour_parse_rgb_value(gdouble *r)
{
    if ( g_scanner_get_next_token(_nk_colour_scanner) != G_TOKEN_FLOAT )
        return FALSE;

    gdouble v = _nk_colour_scanner->value.v_float;
    if ( g_scanner_peek_next_token(_nk_colour_scanner) == '%' )
    {
        g_scanner_get_next_token(_nk_colour_scanner);

        if ( ! IS_BETWEEN(v, 0., 100.) )
            return FALSE;

        *r = v / 100.;
    }
    else
    {
        if ( ! IS_BETWEEN(v, 0., 255.) )
            return FALSE;

        *r = v / 255.;
    }

    return TRUE;
}

static inline gboolean
_nk_colour_parse_hue_value(gdouble *r)
{
    if ( g_scanner_get_next_token(_nk_colour_scanner) != G_TOKEN_FLOAT )
        return FALSE;

    g_scanner_set_scope(_nk_colour_scanner, NK_COLOUR_SCOPE_ANGLES);

    gdouble v = _nk_colour_scanner->value.v_float;
    NkColourAngle unit = NK_COLOUR_ANGLE_DEG;
    if ( g_scanner_peek_next_token(_nk_colour_scanner) == G_TOKEN_SYMBOL )
    {
        g_scanner_get_next_token(_nk_colour_scanner);
        unit = _nk_colour_scanner->value.v_int;
    }
    if ( g_scanner_get_next_token(_nk_colour_scanner) != G_TOKEN_COMMA )
        return FALSE;
    g_scanner_set_scope(_nk_colour_scanner, NK_COLOUR_SCOPE_BASE);

    switch ( unit )
    {
    case NK_COLOUR_ANGLE_DEG:
        v /= 60.;
    break;
    case NK_COLOUR_ANGLE_GRAD:
        v = 3. * v / 200.;
    break;
    case NK_COLOUR_ANGLE_RAD:
        v = 3. * v / G_PI;
    break;
    case NK_COLOUR_ANGLE_TURN:
        v *= 6.;
    break;
    }

    *r = DMODULO(v, 6);

    return TRUE;
}

static inline gboolean
_nk_colour_parse_percent_value(gdouble *r)
{
    if ( g_scanner_get_next_token(_nk_colour_scanner) != G_TOKEN_FLOAT )
        return FALSE;

    gdouble v = _nk_colour_scanner->value.v_float;
    if ( g_scanner_get_next_token(_nk_colour_scanner) != '%' )
        return FALSE;
    if ( ! IS_BETWEEN(v, 0., 100.) )
        return FALSE;

    *r = v / 100.;

    return TRUE;
}

static void
_nk_colour_hsl_to_rgb(NkColour *colour, gdouble h, gdouble s, gdouble l)
{
    gdouble c = ( 1. - ABS(2. * l - 1.) ) * s;
    gdouble hh = DMODULO(h, 2);
    gdouble x = c * ( 1. - ABS(hh - 1.) );
    gdouble m = l - c / 2.;

    gdouble r = 0., g = 0., b = 0.;
    if ( IS_BETWEEN(h, 0., 1.) )
    {
        r = c;
        g = x;
    }
    else if ( IS_BETWEEN(h, 1., 2.) )
    {
        r = x;
        g = c;
    }
    else if ( IS_BETWEEN(h, 2., 3.) )
    {
        g = c;
        b = x;
    }
    else if ( IS_BETWEEN(h, 3., 4.) )
    {
        g = x;
        b = c;
    }
    else if ( IS_BETWEEN(h, 4., 5.) )
    {
        r = x;
        b = c;
    }
    else if ( IS_BETWEEN(h, 5., 6.) )
    {
        r = c;
        b = x;
    }
    colour->red   = r + m;
    colour->green = g + m;
    colour->blue  = b + m;
}

static void
_nk_colour_hwb_to_rgb(NkColour *colour, gdouble h, gdouble w, gdouble b)
{
    _nk_colour_hsl_to_rgb(colour, h, 1., .5);
    colour->red   *= ( 1. - w - b );
    colour->red   += w;
    colour->green *= ( 1. - w - b );
    colour->green += w;
    colour->blue  *= ( 1. - w - b );
    colour->blue  += w;
}

static gboolean
_nk_colour_search_named(NkColour *colour, const gchar *name)
{
    gsize i;
    for ( i = 0 ; i < G_N_ELEMENTS(_nk_colour_scanner_named_colours) ; ++i )
    {
        if ( g_ascii_strcasecmp(name, _nk_colour_scanner_named_colours[i].name) == 0 )
        {
            *colour = _nk_colour_scanner_named_colours[i].value;
            return TRUE;
        }
    }
    return FALSE;
}

gboolean
nk_colour_parse(const gchar *s, NkColour *colour)
{
    if ( _nk_colour_scanner == NULL )
    {
        _nk_colour_scanner = g_scanner_new(NULL);
        _nk_colour_scanner->config->int_2_float = TRUE;
        _nk_colour_scanner->config->cset_identifier_first = "#" G_CSET_a_2_z G_CSET_A_2_Z G_CSET_LATINS G_CSET_LATINC;
        _nk_colour_scanner->config->cpair_comment_single = "";
        _nk_colour_scanner->config->case_sensitive = TRUE;

        gsize i;
        for ( i = 0 ; i < G_N_ELEMENTS(_nk_colour_scanner_symbols_base) ; ++i )
            g_scanner_scope_add_symbol(_nk_colour_scanner, NK_COLOUR_SCOPE_BASE, _nk_colour_scanner_symbols_base[i], GUINT_TO_POINTER(i));
        for ( i = 0 ; i < G_N_ELEMENTS(_nk_colour_scanner_symbols_angle_units) ; ++i )
            g_scanner_scope_add_symbol(_nk_colour_scanner, NK_COLOUR_SCOPE_ANGLES, _nk_colour_scanner_symbols_angle_units[i], GUINT_TO_POINTER(i));
    }

    if ( s == NULL )
        return FALSE;

    g_scanner_input_text(_nk_colour_scanner, s, strlen(s));
    g_scanner_set_scope(_nk_colour_scanner, NK_COLOUR_SCOPE_BASE);

    NkColour colour_ = { .alpha = 1. };
    switch ( g_scanner_get_next_token(_nk_colour_scanner) )
    {
    case G_TOKEN_SYMBOL:
    {
        NkColourSymbolBase symbol = _nk_colour_scanner->value.v_int;
        gboolean alpha = FALSE;
        switch ( symbol )
        {
        case NK_COLOUR_SYMBOL_BASE_RGBA:
            alpha = TRUE;
            /* fallthrough */
        case NK_COLOUR_SYMBOL_BASE_RGB:
            if ( g_scanner_get_next_token(_nk_colour_scanner) != G_TOKEN_LEFT_PAREN )
                return FALSE;
            if ( ! _nk_colour_parse_rgb_value(&colour_.red) )
                return FALSE;
            if ( g_scanner_get_next_token(_nk_colour_scanner) != G_TOKEN_COMMA )
                return FALSE;
            if ( ! _nk_colour_parse_rgb_value(&colour_.green) )
                return FALSE;
            if ( g_scanner_get_next_token(_nk_colour_scanner) != G_TOKEN_COMMA )
                return FALSE;
            if ( ! _nk_colour_parse_rgb_value(&colour_.blue) )
                return FALSE;
            if ( ! _nk_colour_parse_alpha_value(&colour_.alpha, alpha) )
                return FALSE;
            if ( g_scanner_get_next_token(_nk_colour_scanner) != G_TOKEN_RIGHT_PAREN )
                return FALSE;
        break;
        case NK_COLOUR_SYMBOL_BASE_HSLA:
            alpha = TRUE;
            /* fallthrough */
        case NK_COLOUR_SYMBOL_BASE_HSL:
        {
            gdouble h, s, l;
            if ( g_scanner_get_next_token(_nk_colour_scanner) != G_TOKEN_LEFT_PAREN )
                return FALSE;
            if ( ! _nk_colour_parse_hue_value(&h) )
                return FALSE;
            if ( ! _nk_colour_parse_percent_value(&s) )
                return FALSE;
            if ( g_scanner_get_next_token(_nk_colour_scanner) != G_TOKEN_COMMA )
                return FALSE;
            if ( ! _nk_colour_parse_percent_value(&l) )
                return FALSE;
            if ( ! _nk_colour_parse_alpha_value(&colour_.alpha, alpha) )
                return FALSE;
            if ( g_scanner_get_next_token(_nk_colour_scanner) != G_TOKEN_RIGHT_PAREN )
                return FALSE;

            _nk_colour_hsl_to_rgb(&colour_, h, s, l);
        }
        break;
        case NK_COLOUR_SYMBOL_BASE_HWB:
        {
            gdouble h, w, b;
            if ( g_scanner_get_next_token(_nk_colour_scanner) != G_TOKEN_LEFT_PAREN )
                return FALSE;
            if ( ! _nk_colour_parse_hue_value(&h) )
                return FALSE;
            if ( ! _nk_colour_parse_percent_value(&w) )
                return FALSE;
            if ( g_scanner_get_next_token(_nk_colour_scanner) != G_TOKEN_COMMA )
                return FALSE;
            if ( ! _nk_colour_parse_percent_value(&b) )
                return FALSE;
            if ( ! _nk_colour_parse_alpha_value(&colour_.alpha, g_scanner_peek_next_token(_nk_colour_scanner) == G_TOKEN_COMMA) )
                return FALSE;
            if ( g_scanner_get_next_token(_nk_colour_scanner) != G_TOKEN_RIGHT_PAREN )
                return FALSE;

            _nk_colour_hwb_to_rgb(&colour_, h, w, b);
        }
        break;
        }
    }
    break;
    case G_TOKEN_IDENTIFIER:
        if ( _nk_colour_scanner->value.v_identifier[0] == '#' )
        {
            if ( g_scanner_peek_next_token(_nk_colour_scanner) != G_TOKEN_EOF )
                return FALSE;

            const gchar *hex = _nk_colour_scanner->value.v_identifier + strlen("#");
            switch ( strlen(hex) )
            {
            case 8: /* rrggbbaa */
                if ( ! _nk_colour_parse_hex(&colour_.alpha, hex[6], hex[7]) )
                    return FALSE;
                /* fallthrough */
            case 6: /* rrggbb */
                if ( ! _nk_colour_parse_hex(&colour_.red, hex[0], hex[1]) )
                    return FALSE;
                if ( ! _nk_colour_parse_hex(&colour_.green, hex[2], hex[3]) )
                    return FALSE;
                if ( ! _nk_colour_parse_hex(&colour_.blue, hex[4], hex[5]) )
                    return FALSE;
            break;
            case 4: /* rgba */
                if ( ! _nk_colour_parse_hex(&colour_.alpha, hex[3], hex[3]) )
                    return FALSE;
                /* fallthrough */
            case 3: /* rgb */
                if ( ! _nk_colour_parse_hex(&colour_.red, hex[0], hex[0]) )
                    return FALSE;
                if ( ! _nk_colour_parse_hex(&colour_.green, hex[1], hex[1]) )
                    return FALSE;
                if ( ! _nk_colour_parse_hex(&colour_.blue, hex[2], hex[2]) )
                    return FALSE;
            break;
            default:
                return FALSE;
            }
        }
        else if ( ! _nk_colour_search_named(&colour_, _nk_colour_scanner->value.v_identifier) )
            return FALSE;
    break;
    case G_TOKEN_STRING:
        if ( ! _nk_colour_search_named(&colour_, _nk_colour_scanner->value.v_string) )
            return FALSE;
        /* fallthrough */
    default:
        return FALSE;
    }

    if ( g_scanner_get_next_token(_nk_colour_scanner) != G_TOKEN_EOF )
        return FALSE;

    *colour = colour_;

    return TRUE;
}

#define HEX_COLOUR_MAXLEN 10 /* strlen("#rrggbbaa") + 1 */
const gchar *
nk_colour_to_hex(const NkColour *colour)
{
    guint8 red   = (guint8) ( colour->red   * 255. + 0.5 );
    guint8 green = (guint8) ( colour->green * 255. + 0.5 );
    guint8 blue  = (guint8) ( colour->blue  * 255. + 0.5 );
    guint8 alpha = (guint8) ( colour->alpha * 255. + 0.5 );

    static gchar string[HEX_COLOUR_MAXLEN];
    if ( alpha != 0xff )
        g_snprintf(string, HEX_COLOUR_MAXLEN, "#%02x%02x%02x%02x", red, green, blue, alpha);
    else
        g_snprintf(string, HEX_COLOUR_MAXLEN, "#%02x%02x%02x", red, green, blue);
    return string;
}

#define COLOUR_DOUBLE_RGBA_MAXLEN 64 /* strlen("rgba(255.0000000000,255.0000000000,255.0000000000,0.0000000000)") + 1 */
const gchar *
nk_colour_to_rgba(const NkColour *colour)
{
    gdouble red   = colour->red   * 255.;
    gdouble green = colour->green * 255.;
    gdouble blue  = colour->blue  * 255.;
    gdouble alpha = colour->alpha;

    static gchar string[COLOUR_DOUBLE_RGBA_MAXLEN];
    if ( alpha != 1.0 )
        g_snprintf(string, COLOUR_DOUBLE_RGBA_MAXLEN, "rgba(%.10lf,%.10lf,%.10lf,%.10lf)", red, green, blue, alpha);
    else
        g_snprintf(string, COLOUR_DOUBLE_RGBA_MAXLEN, "rgb(%.10lf,%.10lf,%.10lf)", red, green, blue);
    return string;
}

#define COLOUR_DOUBLE_RGBA_MAXLEN 64 /* strlen("rgba(255.0000000000,255.0000000000,255.0000000000,0.0000000000)") + 1 */
const gchar *
nk_colour_to_hsla(const NkColour *colour)
{
    gdouble red   = colour->red   * 255.;
    gdouble green = colour->green * 255.;
    gdouble blue  = colour->blue  * 255.;
    gdouble alpha = colour->alpha;

    static gchar string[COLOUR_DOUBLE_RGBA_MAXLEN];
    if ( alpha != 1.0 )
        g_snprintf(string, COLOUR_DOUBLE_RGBA_MAXLEN, "rgba(%.10lf,%.10lf,%.10lf,%.10lf)", red, green, blue, alpha);
    else
        g_snprintf(string, COLOUR_DOUBLE_RGBA_MAXLEN, "rgb(%.10lf,%.10lf,%.10lf)", red, green, blue);
    return string;
}

