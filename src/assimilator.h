#ifndef ASSIMILATOR_H
#define ASSIMILATOR_H

#include "network-manager.h"


typedef struct {
    gchar * server;
    network_manager_ip4config * network_config;
} server_configuration;


const server_configuration * assimilator_match_network (
        server_configuration ** stored_configurations,
        network_manager_ip4config ** ip_configurations
    );

gboolean assimilator_connect (gchar * configuration_file);

#endif /* ASSIMILATOR_H */
