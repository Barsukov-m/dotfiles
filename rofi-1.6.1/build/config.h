/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Enable libasan */
/* #undef ENABLE_ASAN */

/* Enable desktop file run dialog */
#define ENABLE_DRUN 1

/* Enable gcov profiling */
/* #undef ENABLE_GCOV */

/* The highest GLib version supported */
#define GLIB_VERSION_MAX_ALLOWED (G_ENCODE_VERSION(2,40))

/* The lower GLib version supported */
#define GLIB_VERSION_MIN_REQUIRED (G_ENCODE_VERSION(2,40))

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <locale.h> header file. */
#define HAVE_LOCALE_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* b */
#define NK_XKBCOMMON_HAS_COMPOSE 1

/* b */
#define NK_XKBCOMMON_HAS_CONSUMED2 1

/* Name of package */
#define PACKAGE "rofi"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "https://github.com/davatorium/rofi/"

/* Define to the full name of this package. */
#define PACKAGE_NAME "rofi"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "rofi 1.6.1"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "rofi"

/* Define to the home page for this package. */
#define PACKAGE_URL "https://reddit.com/r/qtools/"

/* Define to the version of this package. */
#define PACKAGE_VERSION "1.6.1"

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Enable extensions on AIX 3, Interix.  */
#ifndef _ALL_SOURCE
# define _ALL_SOURCE 1
#endif
/* Enable GNU extensions on systems that have them.  */
#ifndef _GNU_SOURCE
# define _GNU_SOURCE 1
#endif
/* Enable threading extensions on Solaris.  */
#ifndef _POSIX_PTHREAD_SEMANTICS
# define _POSIX_PTHREAD_SEMANTICS 1
#endif
/* Enable extensions on HP NonStop.  */
#ifndef _TANDEM_SOURCE
# define _TANDEM_SOURCE 1
#endif
/* Enable general extensions on Solaris.  */
#ifndef __EXTENSIONS__
# define __EXTENSIONS__ 1
#endif


/* Version number of package */
#define VERSION "1.6.1"

/* Enable the window mode */
#define WINDOW_MODE 1

/* Define to 1 if `lex' declares `yytext' as a `char *' by default, not a
   `char[]'. */
#define YYTEXT_POINTER 1

/* Define to 1 if on MINIX. */
/* #undef _MINIX */

/* Define to 2 if the system does not provide POSIX.1 features except with
   this defined. */
/* #undef _POSIX_1_SOURCE */

/* Define to 1 if you need to in order for `stat' and other things to work. */
/* #undef _POSIX_SOURCE */

#include "gitconfig.h"
