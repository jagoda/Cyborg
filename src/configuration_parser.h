#ifndef CONFIGURATION_PARSER_H
#define CONFIGURATION_PARSER_H

#include "network-manager.h"


typedef struct {
    gchar * server;
    network_manager_ip4config * network_config;
} server_configuration;


server_configuration ** configuration_parser_load (
        const gchar * configuration_file
    );

void configuration_parser_free_configuration (
        server_configuration * configuration
    );

void configuration_parser_free_configurations (
        server_configuration ** configurations
    );

#endif /* CONFIGURATION_PARSER_H */
