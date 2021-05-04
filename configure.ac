

AC_INIT([libusbmux], [1.0.1], [processor.tools.support@analog.com])

AC_PROG_CC
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_AUX_DIR([.])

AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# Automake >= 1.12 requires AM_PROG_AR when using options -Wall and -Werror.
# To be compatible with older versions of Automake use AM_PROG_AR if it's
# defined only. This line must occur before LT_INIT.
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])

# Initialize libtool
LT_INIT # use ([win32-dll]) for dynamic libraries on Windows

AC_CONFIG_FILES([
    Makefile
    src/Makefile
])
AC_OUTPUT
