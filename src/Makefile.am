AM_CFLAGS = -Wall -Werror -ansi -pedantic

noinst_LIBRARIES = libnetworkmanager.a libnetwork.a

libnetworkmanager_a_SOURCES = network-manager.c network-manager.h
libnetworkmanager_a_CFLAGS = $(AM_CFLAGS) @GIO_CFLAGS@

libnetwork_a_SOURCES = network.c network.h network-manager.h
libnetwork_a_CFLAGS = $(AM_CFLAGS) @GIO_CFLAGS@
