AM_CFLAGS = -Wall -Werror -ansi -pedantic

noinst_LIBRARIES = libnetwork.a

libnetwork_a_SOURCES = network.c network.h \
                       network-manager.c network-manager.h
libnetwork_a_CFLAGS = $(AM_CFLAGS) @GIO_CFLAGS@
