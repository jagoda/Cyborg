AM_CFLAGS = -Wall -Werror

bin_PROGRAMS = cyborg
noinst_LIBRARIES = libnetwork.a libsynergy.a

cyborg_SOURCES = cyborg.c \
                 network.h synergy.h
cyborg_CFLAGS = $(AM_CFLAGS) @GIO_CFLAGS@
cyborg_LDADD = libnetwork.a libsynergy.a @GIO_LIBS@ @JSON_LIBS@

libnetwork_a_SOURCES = network.c network.h \
                       network-manager.c network-manager.h \
                       network_configuration.c network_configuration.h
libnetwork_a_CFLAGS = $(AM_CFLAGS) @GIO_CFLAGS@ @JSON_CFLAGS@

libsynergy_a_SOURCES = synergy.c synergy.h
libsynergy_a_CFLAGS = $(AM_CFLAGS) @GIO_CFLAGS@
