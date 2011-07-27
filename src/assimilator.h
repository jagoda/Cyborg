#ifndef ASSIMILATOR_H
#define ASSIMILATOR_H

#include "network-manager.h"


typedef struct {
    gchar * server;
    network_manager_device_config * device_configuration;
} server_configuration;


const server_configuration * assimilator_match_network (
        server_configuration ** stored_configurations,
        network_manager_device_config ** device_configurations
    );

gboolean assimilator_connect (gchar * configuration_file);

gboolean assimilator_disconnect ();

#endif /* ASSIMILATOR_H */
