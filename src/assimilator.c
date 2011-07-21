#include <glib.h>
#include <stdlib.h>

#include "assimilator.h"
#include "configuration_parser.h"
#include "network-manager.h"
#include "synergy.h"


static server_configuration * match_network(
        network_manager_ip4config * network,
        server_configuration ** configurations
    );

static gboolean network_is_match (
        network_manager_ip4config * reference,
        network_manager_ip4config * target
    );

static guint32 network_address (guint32 address, guint32 prefix);

static gboolean connect_synergy (
        server_configuration ** stored_configurations,
        network_manager_ip4config ** ip_configurations
    );


const server_configuration * assimilator_match_network (
        server_configuration ** stored_configurations,
        network_manager_ip4config ** ip_configurations
    )
{
    network_manager_ip4config ** configuration_pointer;
    server_configuration * matched_configuration;

    if (! stored_configurations)
    {
        g_error("Stored configuration cannot be NULL.");
    }
    if (! ip_configurations)
    {
        g_error("Network configuration cannot be NULL.");
    }

    matched_configuration = NULL;
    for (
            configuration_pointer = ip_configurations;
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
    /* TODO: implement */
    connect_synergy(NULL, NULL);
    return FALSE;
}


server_configuration * match_network(
        network_manager_ip4config * network,
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
                network_is_match(
                    network,
                    (*configuration_pointer)->network_config
                )
            )
        {
            matched_configuration = *configuration_pointer;
            break;
        }
    }

    return matched_configuration;
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
        g_debug("prefix match failed");
    }
    if (reference->gateway_address != target->gateway_address)
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
        g_debug("address match failed");
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
        network_manager_ip4config ** ip_configurations
    )
{
    const server_configuration * matched_configuration;
    gboolean connected = FALSE;

    if ((
            matched_configuration = assimilator_match_network(
                stored_configurations,
                ip_configurations
            )
        ))
    {
        connected = synergy_connect(matched_configuration->server);
    }

    return connected;
}
