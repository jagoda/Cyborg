#include <glib-object.h>
#include <json-glib/json-glib.h>
#include <stdlib.h>

#include "network_configuration.h"

/*
FIXME: need g_type_init called first (currently in network-manager.c). Need to
rework the overall project initialization so that library init does common
stuff and then delegates to submodule initializations.
*/


network_configuration ** network_configuration_parse (const gchar * path)
{
    JsonParser * parser;
    JsonNode * root;
    GError * error;

    g_debug("parsing configuration file '%s'", path);
    parser = json_parser_new();
    json_parser_load_from_file(parser, path, &error);
    if (error)
    {
        g_error("failed to parse file '%s': %s", path, error->message);
        g_error_free(error);
    }
    root = json_parser_get_root(parser);

    /* TODO: implement */

    g_object_unref(parser);
    return NULL;
}

void network_configuration_free (network_configuration ** configurations)
{
    /* TODO: implement */
}
