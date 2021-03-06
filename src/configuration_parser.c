#include <arpa/inet.h>
#include <glib.h>
#include <json-glib/json-glib.h>
#include <netinet/in.h>
#include <pulse/pulseaudio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "configuration_parser.h"
#include "network-manager.h"
#include "types.h"


#define SERVER_KEY      "server"
#define INTERFACE_KEY   "interface"
#define ADDRESS_KEY     "address"
#define PREFIX_KEY      "prefix"
#define GATEWAY_KEY     "gateway"
#define AUDIO_MODE_KEY  "audio_mode"
#define SOURCE_KEY      "audio_source"
#define SINK_KEY        "audio_sink"


static server_configuration * parse_server_configuration (
        JsonObject * json_object
    );

static network_manager_device_config * parse_device_configuration (
        JsonObject * json_object
    );

static pulseaudio_config * parse_audio_configuration (
        JsonObject * json_object
    );


server_configuration ** configuration_parser_load (
        const gchar * configuration_file
    )
{
    JsonParser * parser;
    JsonNode * node_pointer;
    JsonArray * configuration_nodes;
    JsonObject * object_pointer;
    server_configuration ** configurations;
    GError * error;
    guint configuration_count, index;

    if (! configuration_file)
    {
        g_error("Configuration file must not be NULL.");
    }
    parser = json_parser_new();
    configurations = NULL;

    error = NULL;
    if (json_parser_load_from_file(parser, configuration_file, &error))
    {
        node_pointer = json_parser_get_root(parser);

        if (json_node_get_node_type(node_pointer) != JSON_NODE_ARRAY)
        {
            g_error("Expected JSON array for configuration root.");
        }
        configuration_nodes = json_node_get_array(node_pointer);
        configuration_count = json_array_get_length(configuration_nodes);
        configurations = (server_configuration **) g_malloc(
                (configuration_count + 1) * sizeof(server_configuration *)
            );
        for (index = 0; index < configuration_count; index++)
        {
            node_pointer = json_array_get_element(
                    configuration_nodes,
                    index
                );
            if (
                    json_node_get_node_type(node_pointer)
                    !=
                    JSON_NODE_OBJECT
                )
            {
                g_error("Expected configuration to be a JSON object.");
            }
            object_pointer = json_node_get_object(node_pointer);
            configurations[index] = parse_server_configuration(
                    object_pointer
                );
        }
        configurations[index] = NULL;
    }
    else
    {
        g_error(
                "Failed top load configuration file '%s': %s",
                configuration_file,
                error->message
            );
        g_error_free(error);
        error = NULL;
    }

    g_object_unref(parser);

    return configurations;
}

void configuration_parser_free_configuration (
        server_configuration * configuration
    )
{
    if (configuration)
    {
        g_free(configuration->server);
        network_manager_free_device_configuration(
                configuration->device_configuration
            );
        g_free(configuration->audio_configuration);
    }
    g_free(configuration);
}

void configuration_parser_free_configurations (
        server_configuration ** configurations
    )
{
    server_configuration ** configuration_pointer;

    if (configurations)
    {
        for (
                configuration_pointer = configurations;
                *configuration_pointer != NULL;
                configuration_pointer++
            )
        {
            configuration_parser_free_configuration(*configuration_pointer);
        }
        g_free(configurations);
    }
}


server_configuration * parse_server_configuration (
        JsonObject * json_object
    )
{
    server_configuration * configuration = NULL;

    if (json_object == NULL)
    {
        g_error("JSON object must not be NULL.");
    }

    configuration = (server_configuration *) g_malloc(
            sizeof(server_configuration)
        );
    if (! (
            configuration->server = g_strdup(
                json_object_get_string_member(json_object, SERVER_KEY)
            )
        ))
    {
        g_error("Missing server name in configuraiton.");
    }
    configuration->device_configuration = parse_device_configuration(
            json_object
        );
    configuration->audio_configuration = parse_audio_configuration(
            json_object
        );

    return configuration;
}

network_manager_device_config * parse_device_configuration (
        JsonObject * json_object
    )
{
    network_manager_device_config * device_configuration;;
    gchar * server_name, * address_string;
    struct in_addr ip_address;

    if (json_object == NULL)
    {
        g_error("JSON object must not be NULL.");
    }
    if (! (
            server_name
            =
            (gchar *) json_object_get_string_member(json_object, SERVER_KEY)
        ))
    {
        g_error("Missing server name in configuration.");
    }
    device_configuration = (network_manager_device_config *) g_malloc(
            sizeof(network_manager_device_config)
        );
    device_configuration->ip_config = (network_manager_ip4config **) g_malloc(
            2 * sizeof(network_manager_ip4config *)
        );
    device_configuration->ip_config[0] =
            (network_manager_ip4config *) g_malloc(
                sizeof(network_manager_ip4config)
        );
    device_configuration->ip_config[1] = NULL;
    
    if (! (
            device_configuration->device_name = g_strdup(
                json_object_get_string_member(json_object, INTERFACE_KEY)
            )
        ))
    {
        g_error("Invalid interface name for server '%s'", server_name);
    }
    if ((
            address_string
            =
            (gchar *) json_object_get_string_member(json_object, ADDRESS_KEY)
        ))
    {
        if(inet_aton(address_string, &ip_address))
        {
            device_configuration->ip_config[0]->ip_address =
                ntohl(ip_address.s_addr);
        }
        else
        {
            g_error("Invalid source address for server '%s'", server_name);
        }
    }
    else
    {
        g_error("Missing source address for server '%s'", server_name);
    }
    if ((
            address_string
            =
            (gchar *) json_object_get_string_member(json_object, GATEWAY_KEY)
        ))
    {
        if(inet_aton(address_string, &ip_address))
        {
            device_configuration->ip_config[0]->gateway_address =
                ntohl(ip_address.s_addr);
        }
        else
        {
            g_error("Invalid gateway address for server '%s'", server_name);
        }
    }
    else
    {
        g_error("Missing gateway address for server '%s'", server_name);
    }
    device_configuration->ip_config[0]->prefix =
        json_object_get_int_member(json_object, PREFIX_KEY);

    return device_configuration;
}

pulseaudio_config * parse_audio_configuration (JsonObject * json_object)
{
    pulseaudio_config * configuration = NULL;
    gchar * server_name = NULL, * mode = NULL;

    if (! json_object)
    {
        g_error("JSON object must not be NULL.");
    }

    if (! (
            server_name
            =
            (gchar *) json_object_get_string_member(json_object, SERVER_KEY)
        ))
    {
        g_error("Missing server name in configuration.");
    }
    configuration = (pulseaudio_config *) g_malloc(
            sizeof(pulseaudio_config)
        );
    if ((
            mode
            =
            (gchar *) json_object_get_string_member(
                json_object,
                AUDIO_MODE_KEY
            )
        ))
    {
        if (strcmp("push", mode) == 0)
        {
            configuration->mode = PULSEAUDIO_MODE_PUSH;
            configuration->source = PA_INVALID_INDEX;
        }
        else if (strcmp("pull", mode) == 0)
        {
            configuration->mode = PULSEAUDIO_MODE_PULL;
            configuration->source = json_object_get_int_member(
                    json_object,
                    SOURCE_KEY
                );
        }
        else
        {
            g_error("Invalid audio mode for server '%s'", server_name);
        }
    }
    else
    {
        g_error("Missing audio mode for server '%s'", server_name);
    }
    configuration->sink = json_object_get_int_member(
            json_object,
            SINK_KEY
        );

    return configuration;
}
