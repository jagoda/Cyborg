AM_CFLAGS = -Wall -Werror -ansi -pedantic
TESTS = checker

check_PROGRAMS = checker
noinst_LIBRARIES = libnetworkmanager.a libnetwork.a

checker_SOURCES = checker.c network-manager.h \
                  $(top_builddir)/src/network-manager.h
checker_CFLAGS = $(AM_CFLAGS) @CHECK_CFLAGS@
checker_LDADD = libnetworkmanager.a libnetwork.a \
                @CHECK_LIBS@ \
                $(top_builddir)/src/libnetworkmanager.a \
                $(top_builddir)/src/libnetwork.a \
                @GIO_LIBS@ @GLIB_LIBS@

libnetworkmanager_a_SOURCES = network-manager.c network-manager.h \
                              $(top_builddir)/src/network-manager.h
libnetworkmanager_a_CFLAGS = $(AM_CFLAGS) @CHECK_CFLAGS@ @GIO_CFLAGS@
libnetwork_a_SOURCES = network.c network.h \
                       $(top_builddir)/src/network.h
libnetwork_a_CFLAGS = $(AM_CFLAGS) @CHECK_CFLAGS@ @GLIB_CFLAGS@
