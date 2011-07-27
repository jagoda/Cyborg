#include <glib.h>
#include <stdlib.h>
#include <string.h>

#include "assimilator.h"
#include "configuration_parser.h"
#include "network-manager.h"
#include "synergy.h"


static server_configuration * match_network(
        network_manager_device_config * device_configuration,
        server_configuration ** configurations
    );

static gboolean device_is_match (
        network_manager_device_config * reference,
        network_manager_device_config * target
    );

static gboolean network_is_match (
        network_manager_ip4config * reference,
        network_manager_ip4config * target
    );

static guint32 network_address (guint32 address, guint32 prefix);

static gboolean connect_synergy (
        server_configuration ** stored_configurations,
        network_manager_device_config ** device_configurations
    );


const server_configuration * assimilator_match_network (
        server_configuration ** stored_configurations,
        network_manager_device_config ** device_configurations
    )
{
    network_manager_device_config ** configuration_pointer;
    server_configuration * matched_configuration;

    if (! stored_configurations)
    {
        g_error("Stored configuration cannot be NULL.");
    }
    if (! device_configurations)
    {
        g_error("Network configuration cannot be NULL.");
    }

    matched_configuration = NULL;
    for (
            configuration_pointer = device_configurations;
            *configuration_pointer != NULL;
            configuration_pointer++
        )
    {
        if ((
                matched_configuration = match_network(
                    *configuration_pointer,
                    stored_configurations
                )
            ))
        {
            break;
        }
    }

    return matched_configuration;
}

gboolean assimilator_connect (gchar * configuration_file)
{
    server_configuration ** stored_configurations;
    network_manager_device_config ** device_configurations;
    gboolean success;

    if (! configuration_file)
    {
        g_error("Configuration file path must not be NULL.");
    }
    if (! (
            stored_configurations = configuration_parser_load(
                configuration_file
            )
        ))
    {
        g_error("Failed to load configuration file '%s'", configuration_file);
    }
    if (! (
            device_configurations = network_manager_device_configurations()
        ))
    {
        g_error("Failed to get current device configurations.");
    }

    success = TRUE;
    if (! connect_synergy(stored_configurations, device_configurations)) {
        success = FALSE;
    }

    return success;
}

gboolean assimilator_disconnect ()
{
    return synergy_disconnect();
}


server_configuration * match_network(
        network_manager_device_config * device_configuration,
        server_configuration ** configurations
    )
{
    server_configuration ** configuration_pointer;
    server_configuration * matched_configuration;

    matched_configuration = NULL;
    for (
            configuration_pointer = configurations;
            *configuration_pointer != NULL;
            configuration_pointer++
        )
    {
        if (
                device_is_match(
                    device_configuration,
                    (*configuration_pointer)->device_configuration
                )
            )
        {
            matched_configuration = *configuration_pointer;
            break;
        }
    }

    return matched_configuration;
}

gboolean device_is_match(
        network_manager_device_config * reference,
        network_manager_device_config * target
    )
{
    network_manager_ip4config ** configuration_pointer;
    gboolean is_match;

    is_match = FALSE;
    if (strcmp(reference->device_name, target->device_name) == 0)
    {
        /* NOTE: server configurations (from config file) should only
           contain a single IP configuration whereas network manager
           generated device configurations can technically have multiple
           IP configurations. */
        for (
                configuration_pointer = target->ip_config;
                *configuration_pointer != NULL;
                configuration_pointer++
            )
        {
            if (
                    network_is_match(
                        *(reference->ip_config),
                        *configuration_pointer
                    )
                )
            {
                is_match = TRUE;
                break;
            }
        }
    }

    return is_match;
}

gboolean network_is_match (
        network_manager_ip4config * reference,
        network_manager_ip4config * target
    )
{
    gboolean is_match;

    is_match = TRUE;
    if (reference->prefix != target->prefix)
    {
        is_match = FALSE;
    }
    if (
            reference->gateway_address != target->gateway_address
        )
    {
        is_match = FALSE;
    }
    if (
            network_address(reference->ip_address, reference->prefix)
            !=
            network_address(target->ip_address, target->prefix)
        )
    {
        is_match = FALSE;
    }

    return is_match;
}

guint32 network_address (guint32 address, guint32 prefix)
{
    guint32 network_mask = 0xFFFFFFFF << (32 - prefix);
    return address & network_mask;
}

gboolean connect_synergy (
        server_configuration ** stored_configurations,
        network_manager_device_config ** device_configurations
    )
{
    const server_configuration * matched_configuration;
    gboolean connected = FALSE;

    if ((
            matched_configuration = assimilator_match_network(
                stored_configurations,
                device_configurations
            )
        ))
    {
        connected = synergy_connect(matched_configuration->server);
    }

    return connected;
}
