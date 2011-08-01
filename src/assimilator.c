#include <glib.h>
#include <pulse/pulseaudio.h>
#include <stdlib.h>
#include <string.h>

#include "assimilator.h"
#include "configuration_parser.h"
#include "network-manager.h"
#include "synergy.h"


static GQueue * pulseaudio_modules = NULL;


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
    server_configuration * matched_configuration;
    network_manager_device_config ** device_configurations;
    gboolean success = FALSE;

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

    if ((
            matched_configuration =
                (server_configuration *) assimilator_match_network(
                    stored_configurations,
                    device_configurations
                )
        ))
    {
        success = TRUE;
        if (! synergy_connect(matched_configuration))
        {
            success = FALSE;
            g_warning("Failed to connect to Synergy.");
        }
        if (pulseaudio_modules == NULL)
        {
            pulseaudio_modules = g_queue_new();
            if (!
                    pulseaudio_connect(
                        matched_configuration,
                        pulseaudio_modules
                    )
                )
            {
                success = FALSE;
                g_warning("Failed to connect to PulseAudio.");
            }
        }
        else
        {
            g_error("PulseAudio already connected.");
        }
    }

    configuration_parser_free_configurations(stored_configurations);
    network_manager_free_device_configurations(device_configurations);

    return success;
}

gboolean assimilator_disconnect ()
{
    gboolean success = FALSE;

    if (pulseaudio_modules != NULL)
    {
        success = 
            synergy_disconnect()
            &&
            pulseaudio_disconnect(pulseaudio_modules);

        g_queue_free(pulseaudio_modules);
        pulseaudio_modules = NULL;
    }

    return success;
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
