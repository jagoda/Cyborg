TESTS = checker

check_PROGRAMS = checker
noinst_LIBRARIES = libnetworkmanager.a

checker_SOURCES = checker.c network-manager.h \
                  $(top_builddir)/src/network-manager.h
checker_CFLAGS = @CHECK_CFLAGS@
checker_LDADD = libnetworkmanager.a @CHECK_LIBS@ \
                $(top_builddir)/src/libnetworkmanager.a \
                @GIO_LIBS@

libnetworkmanager_a_SOURCES = network-manager.c network-manager.h \
                              $(top_builddir)/src/network-manager.h
libnetworkmanager_a_CFLAGS = @CHECK_CFLAGS@ @GIO_CFLAGS@
