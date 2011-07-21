AM_CFLAGS = -Wall -Werror

bin_PROGRAMS = cyborg
pkglib_LTLIBRARIES = libnetwork.la


cyborg_SOURCES = cyborg.c
cyborg_CFLAGS = $(AM_CFLAGS) @GLIB_CFLAGS@
cyborg_LDADD = libnetwork.la @GLIB_LIBS@ @JSON_LIBS@

libnetwork_la_SOURCES = assimilator.c assimilator.h \
                        configuration_parser.c configuration_parser.h \
                        network-manager.c network-manager.h \
                        synergy.c synergy.h
libnetwork_la_CFLAGS = $(AM_CFLAGS) @GLIB_CFLAGS@ @JSON_CFLAGS@
