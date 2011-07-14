AM_CFLAGS = -Wall -Werror

noinst_LIBRARIES = libnetwork.a

libnetwork_a_SOURCES = network.c network.h \
                       network-manager.c network-manager.h \
                       network_configuration.c network_configuration.h
libnetwork_a_CFLAGS = $(AM_CFLAGS) @GIO_CFLAGS@ @JSON_CFLAGS@
