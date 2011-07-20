AM_CFLAGS = -Wall -Werror

pkglib_LTLIBRARIES = libnetwork.la


libnetwork_la_SOURCES = network-manager.c network-manager.h \
                        configuration_parser.c configuration_parser.h
libnetwork_la_CFLAGS = $(AM_CFLAGS) @GLIB_CFLAGS@ @JSON_CFLAGS@
