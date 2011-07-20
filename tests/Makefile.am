AM_CFLAGS = -Wall -Werror

TESTS = checker
check_PROGRAMS = checker
noinst_LTLIBRARIES = network-manager.la \
                     configuration_parser.la


checker_SOURCES = checker.c
checker_CFLAGS = $(AM_CFLAGS) @GLIB_CFLAGS@
checker_LDFLAGS = -lltdl @GLIB_LIBS@ @JSON_LIBS@


network_manager_la_SOURCES = network-manager.c \
                             $(top_builddir)/src/network-manager.h
network_manager_la_CFLAGS = $(AM_CFLAGS) @GLIB_CFLAGS@
network_manager_la_LDFLAGS = -module -rpath $(libdir)
network_manager_la_LIBADD = $(top_builddir)/src/libnetwork.la

configuration_parser_la_SOURCES = configuration_parser.c \
                                  $(top_builddir)/src/configuration_parser.h
configuration_parser_la_CFLAGS = $(AM_CFLAGS) @GLIB_CFLAGS@
configuration_parser_la_LDFLAGS = -module -rpath $(libdir)
configuration_parser_la_LIBADD = $(top_builddir)/src/libnetwork.la
