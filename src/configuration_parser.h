#ifndef CONFIGURATION_PARSER_H
#define CONFIGURATION_PARSER_H

#include "assimilator.h"
#include "network-manager.h"


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
