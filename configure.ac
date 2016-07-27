AC_COPYRIGHT([Copyright (C) 2016 Santtu Lakkala])
AC_INIT([geoclue-mock], [0.0.2], [inz@inz.fi])
AC_PREREQ(2.60)

AC_CONFIG_SRCDIR(configure.ac)
AC_CONFIG_AUX_DIR(admin)
AC_CONFIG_MACRO_DIR(m4)
AC_CONFIG_HEADER(config.h)
AC_CONFIG_FILES([
	Makefile
	src/Makefile
])


AC_PROG_CC

AC_USE_SYSTEM_EXTENSIONS
AC_C_RESTRICT
CFLAGS="${CFLAGS} -Wall -Wextra"
CFLAGS="${CFLAGS} -Wundef -Wnested-externs"
CFLAGS="${CFLAGS} -Wwrite-strings"
CFLAGS="${CFLAGS} -Wpointer-arith -Wbad-function-cast"
CFLAGS="${CFLAGS} -Wmissing-declarations -Wmissing-prototypes"
CFLAGS="${CFLAGS} -Wredundant-decls"
CFLAGS="${CFLAGS} -Wunused-parameter -Wold-style-definition"
CFLAGS="${CFLAGS} -Winline"

AC_DEFINE(_FORTIFY_SOURCE, 2, [Define to 2 to get glibc warnings.])

AC_HEADER_ASSERT

AM_INIT_AUTOMAKE([1.11 color-tests])
AM_SILENT_RULES([yes])

AM_PROG_CC_C_O
AC_DISABLE_STATIC
AM_PROG_LIBTOOL

PKG_CHECK_MODULES(GLIB, [glib-2.0 gio-2.0])
PKG_CHECK_MODULES(GEOCLUE, [geoclue])

GLIB_GSETTINGS

DBUS_SERVICES_DIR=`$PKG_CONFIG dbus-1 --variable=session_bus_services_dir`
AC_SUBST(DBUS_SERVICES_DIR)

AC_OUTPUT
