/*
 * rofi
 *
 * MIT/X11 License
 * Copyright © 2013-2020 Qball Cow <qball@gmpclient.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef ROFI_HELPER_THEME_H
#define ROFI_HELPER_THEME_H
#include <pango/pango.h>
#include "theme.h"
/**
 * @defgroup HELPERS Helpers
 * @{
 */
/**
 * @param th The RofiHighlightColorStyle
 * @param tokens Array of regexes used for matching
 * @param input The input string to find the matches on
 * @param retv The Attribute list to update with matches
 *
 * Creates a set of pango attributes highlighting the matches found in the input string.
 *
 * @returns the updated retv list.
 */
PangoAttrList *helper_token_match_get_pango_attr ( RofiHighlightColorStyle th, rofi_int_matcher **tokens, const char *input, PangoAttrList *retv );

/**
 * @param pfd Pango font description to validate.
 * @param font The name of the font to check.
 *
 * @returns true if font is valid.
 */
gboolean helper_validate_font ( PangoFontDescription *pfd, const char *font );
/** @} */
#endif // ROFI_HELPER_THEME_H
