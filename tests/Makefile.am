AM_CFLAGS = -Wall -Werror
TESTS = checker

check_PROGRAMS = checker
noinst_LIBRARIES = libnetwork.a

checker_SOURCES = checker.c network-manager.h \
                  $(top_builddir)/src/network-manager.h
checker_CFLAGS = $(AM_CFLAGS) @CHECK_CFLAGS@
checker_LDADD = libnetwork.a \
                $(top_builddir)/src/libnetwork.a \
                @CHECK_LIBS@ @GIO_LIBS@ @JSON_LIBS@

libnetwork_a_SOURCES = network-manager.c network-manager.h \
                       network.c network.h \
                       network_configuration.c network_configuration.h \
                       $(top_builddir)/src/network_configuration.h \
                       $(top_builddir)/src/network-manager.h \
                       $(top_builddir)/src/network.h
libnetwork_a_CFLAGS = $(AM_CFLAGS) @CHECK_CFLAGS@ @GIO_CFLAGS@
