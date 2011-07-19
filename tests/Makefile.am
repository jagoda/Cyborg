AM_CFLAGS = -Wall -Werror

TESTS = checker
check_PROGRAMS = checker
noinst_LTLIBRARIES = network-manager.la


checker_SOURCES = checker.c
checker_CFLAGS = $(AM_CLFAGS) @GLIB_CFLAGS@
checker_LDADD = @GLIB_LIBS@


network_manager_la_SOURCES = network-manager.c test_api.h
network_manager_la_CFLAGS = $(AM_CFLAGS) @GLIB_CFLAGS@
network_manager_la_LDFLAGS = -module -rpath $(libdir)
