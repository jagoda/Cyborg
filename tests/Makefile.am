AM_CFLAGS = -Wall -Werror

TESTS = checker
check_PROGRAMS = checker
noinst_LTLIBRARIES = assimilator.la \
                     configuration_parser.la \
                     network-manager.la \
                     synergy.la \
                     pulseaudio.la


checker_SOURCES = checker.c
checker_CFLAGS = $(AM_CFLAGS) @GLIB_CFLAGS@
checker_LDFLAGS = -lltdl @GLIB_LIBS@ @JSON_LIBS@ @PULSEAUDIO_LIBS@


assimilator_la_SOURCES = assimilator.c \
                         $(top_builddir)/src/assimilator.h \
                         $(top_builddir)/src/configuration_parser.h \
                         $(top_builddir)/src/network-manager.h
assimilator_la_CFLAGS = $(AM_CFLAGS) @GLIB_CFLAGS@
assimilator_la_LDFLAGS = -module -rpath $(libdir)
assimilator_la_LIBADD = $(top_builddir)/src/libnetwork.la

configuration_parser_la_SOURCES = configuration_parser.c \
                                  $(top_builddir)/src/configuration_parser.h
configuration_parser_la_CFLAGS = $(AM_CFLAGS) @GLIB_CFLAGS@
configuration_parser_la_LDFLAGS = -module -rpath $(libdir)
configuration_parser_la_LIBADD = $(top_builddir)/src/libnetwork.la

network_manager_la_SOURCES = network-manager.c \
                             $(top_builddir)/src/network-manager.h
network_manager_la_CFLAGS = $(AM_CFLAGS) @GLIB_CFLAGS@
network_manager_la_LDFLAGS = -module -rpath $(libdir)
network_manager_la_LIBADD = $(top_builddir)/src/libnetwork.la

synergy_la_SOURCES = synergy.c \
                     $(top_builddir)/src/synergy.h
synergy_la_CFLAGS = $(AM_CFLAGS) @GLIB_CFLAGS@
synergy_la_LDFLAGS = -module -rpath $(libdir)
synergy_la_LIBADD = $(top_builddir)/src/libnetwork.la

pulseaudio_la_SOURCES = pulseaudio.c \
                        $(top_builddir)/src/pulseaudio.h
pulseaudio_la_CFLAGS = $(AM_CFLAGS) @GLIB_CFLAGS@
pulseaudio_la_LDFLAGS = -module -rpath $(libdir)
pulseaudio_la_LIBADD = $(top_builddir)/src/libnetwork.la
