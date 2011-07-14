#include <gio/gio.h>
#include <glib.h>
#include <glib-object.h>
#include <stdlib.h>

#include "network.h"
#include "network-manager.h"
#include "synergy.h"


static const gchar * DEFAULT_CONFIGURATION = "/.cyborg.conf";


int main (int argc, char ** argv)
{
    GDBusConnection * dbus_connection;
    network_configuration ** network_configurations;
    gchar * cyborg_configuration, * home, * server;

    g_type_init();

    home = getenv("HOME");
    cyborg_configuration = g_strconcat(home, DEFAULT_CONFIGURATION, NULL);
    
    g_debug(
            "attempting to load configuration file '%s'",
            cyborg_configuration
        );
    network_configurations = network_configuration_parse(cyborg_configuration);
    g_free(cyborg_configuration);

    g_debug("asking DBus for status from devices");
    dbus_connection = network_manager_init();
    ip4_config ** device_addresses = network_my_addresses(dbus_connection);
    g_dbus_connection_close_sync(dbus_connection, NULL, NULL);

    g_debug("attempting to match servers");
    server = network_configuration_match(
            network_configurations,
            device_addresses
        );
    network_configuration_free(network_configurations);
    network_manager_free_addresses(device_addresses);

    if (server)
    {
        g_debug("found server '%s'", server);
        synergy_connect(server);
    }
    else
    {
        g_error("no servers found");
    }

    return EXIT_SUCCESS;
}
