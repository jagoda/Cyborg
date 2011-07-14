#include <glib-object.h>
#include <json-glib/json-glib.h>
#include <stdlib.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include "network.h"


static const gchar * SERVER_KEY  = "server";
static const gchar * ADDRESS_KEY = "address";
static const gchar * PREFIX_KEY  = "prefix";
static const gchar * GATEWAY_KEY = "gateway";


static guint32 ip_to_bytes (const gchar * address);

static gchar * find_match (
        network_configuration ** configurations,
        ip4_config * ip_config
    );

static gboolean is_match (
        network_configuration * configuration,
        ip4_config * ip_config
    );

static guint32 network_address (guint32 address, guint32 prefix);


network_configuration ** network_configuration_parse (const gchar * path)
{
    JsonParser * parser;
    JsonNode * root;
    JsonArray * configuration_nodes;
    JsonObject * configuration_node;
    GError * error;
    network_configuration ** configurations, * configuration;
    guint configuration_count, index;

    g_debug("parsing configuration file '%s'", path);
    error = NULL;
    parser = json_parser_new();
    json_parser_load_from_file(parser, path, &error);
    if (error)
    {
        g_error("failed to parse file '%s': %s", path, error->message);
        g_error_free(error);
    }
    root = json_parser_get_root(parser);

    g_debug("walking json tree to extract configurations");
    configuration_nodes = json_node_get_array(root);
    configuration_count = json_array_get_length(configuration_nodes);
    configurations = (network_configuration **) g_malloc(
            (configuration_count + 1) * sizeof(network_configuration *)
        );
    for (index = 0; index < configuration_count; index++)
    {
        configurations[index] = (network_configuration *) g_malloc(
                sizeof(network_configuration)
            );
        configuration = configurations[index];
        configuration_node = json_array_get_object_element(
                configuration_nodes,
                index
            );
        configuration->server = g_strdup(
                json_object_get_string_member(configuration_node, SERVER_KEY)
            );
        g_debug("found configuration for '%s'", configuration->server);
        configuration->prefix =
            (guint) json_object_get_int_member(configuration_node, PREFIX_KEY);
        g_debug("configuration prefix is %d", configuration->prefix);
        configuration->address =
            ip_to_bytes(
                    json_object_get_string_member(
                        configuration_node,
                        ADDRESS_KEY
                    )
                );
        configuration->gateway =
            ip_to_bytes(
                    json_object_get_string_member(
                        configuration_node,
                        GATEWAY_KEY
                    )
                );
    }
    configurations[index] = NULL;
    g_object_unref(parser);

    return configurations;
}

void network_configuration_free (network_configuration ** configurations)
{
    network_configuration ** configuration_pointer;

    g_debug("deallocating network configuration resources");
    for (
            configuration_pointer = configurations;
            *configuration_pointer != NULL;
            configuration_pointer++
        )
    {
        g_free(*configuration_pointer);
    }
    g_free(configurations);
}

gchar * network_configuration_match (
        network_configuration ** network_configurations,
        ip4_config ** network_status
    )
{
    ip4_config ** ip_pointer;
    gchar * server;

    g_debug("iterating over ip configs for matching");
    server = NULL;
    for (
            ip_pointer = network_status;
            *ip_pointer != NULL && server == NULL;
            ip_pointer++
        )
    {
        server = find_match(network_configurations, *ip_pointer);
    }

    return server;
}


guint32 ip_to_bytes (const gchar * address)
{
    struct in_addr buffer;

    g_debug("converting '%s' to byes", address);
    inet_aton(address, &buffer);
    return (guint32) ntohl(buffer.s_addr);
}

gchar * find_match (
        network_configuration ** configurations,
        ip4_config * ip_config
    )
{
    network_configuration ** configuration_pointer;
    gchar * server;

    g_debug("searching for configuration match for ip status");
    server = NULL;
    for (
            configuration_pointer = configurations;
            *configuration_pointer != NULL;
            configuration_pointer++
        )
    {
        if (is_match(*configuration_pointer, ip_config))
        {
            g_debug("found config match for '%s'",
                    (*configuration_pointer)->server
                );
            server = (*configuration_pointer)->server;
            break;
        }
    }

    return server;
}

gboolean is_match (
        network_configuration * configuration,
        ip4_config * ip_config
    )
{
    gboolean is_match = TRUE;

    g_debug("checking for match between ip config and network configuration");
    if (configuration->gateway != ip_config->gateway)
    {
        is_match = FALSE;
    }
    if (configuration->prefix != ip_config->prefix)
    {
        is_match = FALSE;
    }
    if (network_address(configuration->address, configuration->prefix) !=
            network_address(ip_config->address, ip_config->prefix))
    {
        is_match = FALSE;
    }

    return is_match;
}

guint32 network_address (guint32 address, guint32 prefix)
{
    guint32 network_address, mask;

    g_debug("computing network address from address/prefix pair");
    mask = 0xFFFFFFFF << (32 - prefix);
    network_address = address & mask;

    return network_address;
}
