dnl
dnl libnkutils - Miscellaneous utilities
dnl
dnl Copyright © 2011-2017 Quentin "Sardem FF7" Glidic
dnl
dnl Permission is hereby granted, free of charge, to any person obtaining a copy
dnl of this software and associated documentation files (the "Software"), to deal
dnl in the Software without restriction, including without limitation the rights
dnl to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
dnl copies of the Software, and to permit persons to whom the Software is
dnl furnished to do so, subject to the following conditions:
dnl
dnl The above copyright notice and this permission notice shall be included in
dnl all copies or substantial portions of the Software.
dnl
dnl THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
dnl IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
dnl FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
dnl AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
dnl LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
dnl OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
dnl THE SOFTWARE.
dnl


# NK_INIT([modules])
#     modules                       A list of modules to enable (shorthand for NK_ENABLE_MODULES)
AC_DEFUN([NK_INIT], [
    AC_REQUIRE([AC_HEADER_STDC])
    AC_REQUIRE([PKG_PROG_PKG_CONFIG])

    nk_glib_min_version="2.40"
    nk_xkbcommon_min_version="0.4.1"

    m4_map_args_w(_NK_MODULES, [_NK_MODULE_INIT(AS_TR_SH(], [))])
    AC_CONFIG_COMMANDS_PRE([
        AC_CHECK_HEADERS([locale.h])
        AS_IF([test x${ac_cv_header_string_h} != xyes], [AC_MSG_ERROR([libnkutils: string.h is required])])
        PKG_CHECK_MODULES([_NKUTILS_INTERNAL_GLIB], [glib-2.0 >= ${nk_glib_min_version}])
        PKG_CHECK_MODULES([_NKUTILS_INTERNAL_TEST], [gobject-2.0])
        m4_map_args_w(_NK_MODULES, [_NK_MODULE_CHECK(], [)])
    ])

    m4_ifnblank([$1], [NK_ENABLE_MODULES([$1])])
    AM_XSLTPROCFLAGS="${AM_XSLTPROCFLAGS} "'${NKUTILS_XSLTPROCFLAGS}'

    AM_CONDITIONAL([NK_ENABLE_UUID_LIBUUID], [test x${nk_uuid_libuuid} = xyes])
    AM_CONDITIONAL([NK_ENABLE_UUID_APR_UTIL], [test x${nk_uuid_apr_util} = xyes])

    m4_define([NK_INIT])
])

# NK_ENABLE_MODULES(modules)
#     modules  A list of modules to enable
AC_DEFUN([NK_ENABLE_MODULES], [
    m4_map_args_w([$1], [_NK_ENABLE_MODULE(], [)])
])

m4_define([_NK_MODULES], [uuid enum token colour gtk-settings xdg-de xdg-theme bindings])


# auto-enable
m4_define([_nk_dependent_enum], [token xdg-de xdg-theme bindings])
m4_define([_nk_dependent_gtk_settings], [xdg-theme bindings])
m4_define([_nk_dependent_xdg_de], [xdg-theme bindings])



AC_DEFUN([_NK_MODULE_INIT], [
    nk_module_][$1][_enable=no
])

AC_DEFUN([_NK_MODULE_CHECK], [
    AM_CONDITIONAL([NK_ENABLE_]AS_TR_CPP([$1]), [test x${nk_module_]AS_TR_SH([$1])[_enable} = xyes]_NK_MODULE_CHECK_DEPENDENT([_nk_dependent_]AS_TR_SH([$1])))
])

AC_DEFUN([_NK_MODULE_CHECK_DEPENDENT], [m4_ifdef([$1], m4_map_args_w($1, [[ -o x${nk_module_]AS_TR_SH(], [)[_enable} = xyes]]))])


AC_DEFUN([_NK_ENABLE_MODULE], [
    m4_if(m4_index(_NK_MODULES, [$1]), [-1], [AC_MSG_ERROR([libnkutils: No ][$1][ module])])
    m4_ifdef([_NK_]AS_TR_CPP([$1])[_CHECK], [_NK_]AS_TR_CPP([$1])[_CHECK])
    [nk_module_]AS_TR_SH([$1])[_enable=yes]
])

# Special dependencies
AC_DEFUN([_NK_UUID_CHECK], [
    PKG_CHECK_MODULES([_NKUTILS_INTERNAL_UUID_LIBUUID], [uuid], [nk_uuid_libuuid=yes], [nk_uuid_libuuid=no])
    PKG_CHECK_MODULES([_NKUTILS_INTERNAL_UUID_APR_UTIL], [apr-util-1], [nk_uuid_apr_util=yes], [
        PKG_CHECK_MODULES([_NKUTILS_INTERNAL_UUID_APR_UTIL], [apr-util], [nk_uuid_apr_util=yes], [nk_uuid_apr_util=no])
    ])
    AS_IF([test x${nk_uuid_libuuid} != xyes -a x${nk_uuid_apr_util} != xyes], [AC_MSG_ERROR([libnkutils: A UUID library is required])])
])

AC_DEFUN([_NK_XDG_THEME_CHECK], [
    PKG_CHECK_MODULES([_NKUTILS_INTERNAL_GIO], [gio-2.0])
    PKG_CHECK_MODULES([_NKUTILS_INTERNAL_GOBJECT], [gobject-2.0])
])

AC_DEFUN([_NK_BINDINGS_CHECK], [
    PKG_CHECK_MODULES([_NKUTILS_INTERNAL_XKBCOMMON], [xkbcommon >= ${nk_xkbcommon_min_version}])
    PKG_CHECK_EXISTS([xkbcommon >= 0.7.0], [
        AC_DEFINE([NK_XKBCOMMON_HAS_COMPOSE], [1], [b])
        AC_DEFINE([NK_XKBCOMMON_HAS_CONSUMED2], [1], [b])
    ], [
        PKG_CHECK_EXISTS([xkbcommon >= 0.5.0], [
            AC_DEFINE([NK_XKBCOMMON_HAS_COMPOSE], [1], [b])
        ])
    ])
])
