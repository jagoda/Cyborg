#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.67])
AC_INIT([Cyborg], [0.2], [jeffrey.jagoda@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_SRCDIR([tests/checker.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_LIBTOOL
AC_PROG_MAKE_SET
PKG_CHECK_MODULES([GLIB], [glib-2.0 >= 2.28.6, gio-2.0 >= 2.28.6])
PKG_CHECK_MODULES([JSON], [json-glib-1.0 >= 0.12.2])

# Checks for libraries.
AC_CHECK_LIB([ltdl], [lt_dlinit])

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h netinet/in.h stdlib.h string.h sys/socket.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 tests/Makefile])
AC_OUTPUT
