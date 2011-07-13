#ifndef NETWORK_CONFIGURATION_H
#define NETWORK_CONFIGURATION_H

#include <glib.h>


typedef struct {
    gchar * server;
    guint32 address;
    guint32 prefix;
    guint32 gateway;
} network_configuration;


network_configuration ** network_configuration_parse (const gchar * path);

void network_configuration_free (network_configuration ** configurations);

#endif /* NETWORK_CONFIGURATION_H */
