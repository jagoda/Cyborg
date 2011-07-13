#include <glib-object.h>
#include <json-glib/json-glib.h>
#include <stdlib.h>

#include "network.h"


static const gchar * SERVER_KEY  = "server";
/*
static const gchar * ADDRESS_KEY = "address";
*/
static const gchar * PREFIX_KEY  = "prefix";
/*
static const gchar * GATEWAY_KEY = "gateway";
*/


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
        /* FIXME: need to parse IPs to ints. */
    }
    configurations[index] = NULL;
    g_object_unref(parser);

    return configurations;
}

void network_configuration_free (network_configuration ** configurations)
{
    network_configuration ** configuration_pointer;

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
