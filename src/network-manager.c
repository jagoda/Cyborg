#include <gio/gio.h>
#include <glib.h>
#include <glib-object.h>
#include <arpa/inet.h>
#include <stdlib.h>

#include "network.h"


/* DBus constants */
static const char * PROPERTIES_INTERFACE = "org.freedesktop.DBus.Properties";

/* Network Manager constants */
static const char * SERVICE = "org.freedesktop.NetworkManager";

static const char * MANAGER_OBJECT = "/org/freedesktop/NetworkManager";

static const char * DEVICE_INTERFACE = "org.freedesktop.NetworkManager.Device";
static const char * IP4CONFIG_INTERFACE = 
    "org.freedesktop.NetworkManager.IP4Config";
static const char * MANAGER_INTERFACE = "org.freedesktop.NetworkManager";


GDBusConnection * network_manager_init ()
{
    GDBusConnection * connection;

    network_init();
    g_debug("connecting to the system bus");
    connection = g_bus_get_sync(
            G_BUS_TYPE_SYSTEM,
            NULL,
            NULL
        );

    return connection;
}

gchar ** network_manager_get_devices (GDBusConnection * connection)
{
    GVariant * dbus_response;
    GVariantIter * object_paths;
    gchar ** devices, ** path_pointer;

    g_debug("asking network manager for devices");
    dbus_response = g_dbus_connection_call_sync(
            connection,
            SERVICE,
            MANAGER_OBJECT,
            MANAGER_INTERFACE,
            "GetDevices",
            NULL,
            G_VARIANT_TYPE("(ao)"),
            G_DBUS_CALL_FLAGS_NONE,
            -1,
            NULL,
            NULL
        );

    g_debug("extracting device list from response");
    g_variant_get(dbus_response, "(ao)", &object_paths);
    g_variant_unref(dbus_response);
    devices = (gchar **) g_malloc(
            (g_variant_iter_n_children(object_paths) + 1) * sizeof(gchar *)
        );
    for (
            path_pointer = devices;
            g_variant_iter_next(object_paths, "o", path_pointer);
            path_pointer++
        );
    *path_pointer = NULL;
    g_variant_iter_free(object_paths);

    return devices;
}

void network_manager_free_devices (gchar ** devices)
{
    gchar ** device_pointer;

    for (device_pointer = devices; *device_pointer != NULL; device_pointer++)
    {
        g_free(*device_pointer);
    }
    g_free(devices);
}

gint network_manager_device_state (
        GDBusConnection * connection,
        gchar * device
    )
{
    GVariant * dbus_parameters, * dbus_response, * response_payload;
    gint device_state;

    g_debug("asking DBus for device state for '%s'", device);
    dbus_parameters = g_variant_new("(ss)", DEVICE_INTERFACE, "State");
    dbus_response = g_dbus_connection_call_sync(
            connection,
            SERVICE,
            device,
            PROPERTIES_INTERFACE,
            "Get",
            dbus_parameters,
            G_VARIANT_TYPE("(v)"),
            G_DBUS_CALL_FLAGS_NONE,
            -1,
            NULL,
            NULL
        );
    g_variant_unref(dbus_parameters);

    g_debug("unpacking device state from response");
    g_variant_get(dbus_response, "(v)", &response_payload);
    g_variant_unref(dbus_response);
    device_state = g_variant_get_uint32(response_payload);
    g_variant_unref(response_payload);

    return device_state;
}

ip4_config ** network_manager_device_addresses (
        GDBusConnection * connection,
        gchar * device
    )
{
    GVariant * dbus_parameters, * dbus_response, * response_payload;
    GVariantIter * address_tuples, * address_parts;
    ip4_config ** addresses, ** address_pointer, * ip_config;
    const gchar * config_path;

    g_debug("asking DBus for Ip4Config for '%s'", device);
    dbus_parameters = g_variant_new("(ss)", DEVICE_INTERFACE, "Ip4Config");
    dbus_response = g_dbus_connection_call_sync(
            connection,
            SERVICE,
            device,
            PROPERTIES_INTERFACE,
            "Get",
            dbus_parameters,
            G_VARIANT_TYPE("(v)"),
            G_DBUS_CALL_FLAGS_NONE,
            -1,
            NULL,
            NULL
        );
    g_variant_unref(dbus_parameters);

    g_debug("extracting config path from response");
    g_variant_get(dbus_response, "(v)", &response_payload);
    g_variant_unref(dbus_response);
    config_path = g_variant_get_string(response_payload, NULL);

    g_debug("asking DBus for address list for '%s'", config_path);
    dbus_parameters = g_variant_new("(ss)", IP4CONFIG_INTERFACE, "Addresses");
    dbus_response = g_dbus_connection_call_sync(
            connection,
            SERVICE,
            config_path,
            PROPERTIES_INTERFACE,
            "Get",
            dbus_parameters,
            G_VARIANT_TYPE("(v)"),
            G_DBUS_CALL_FLAGS_NONE,
            -1,
            NULL,
            NULL
        );
    g_variant_unref(dbus_parameters);
    /* Wait to free this because config_path is stored here... */
    g_variant_unref(response_payload);

    g_debug("extracting device addresses from response");
    g_variant_get(dbus_response, "(v)", &response_payload);
    g_variant_unref(dbus_response);
    g_variant_get(response_payload, "aau", &address_tuples);
    g_variant_unref(response_payload);

    g_debug("parsing address tuple");
    addresses = (ip4_config **) g_malloc(
            (g_variant_iter_n_children(address_tuples) + 1) *
                sizeof(ip4_config *)
        );
    for (
            address_pointer = addresses;
            g_variant_iter_next(address_tuples, "au", &address_parts);
            address_pointer++
        )
    {
        *address_pointer = ip_config = (ip4_config *) g_malloc(sizeof(ip4_config));
        g_variant_iter_next(address_parts, "u", &ip_config->address);
        g_variant_iter_next(address_parts, "u", &ip_config->prefix);
        g_variant_iter_next(address_parts, "u", &ip_config->gateway);

        /*
        ip_config->address = ntohl(ip_config->address);
        ip_config->prefix = ntohl(ip_config->prefix);
        ip_config->gateway = ntohl(ip_config->gateway);
        */
        g_debug("Network prefix: %d", ip_config->prefix);
    }
    *address_pointer = NULL;
    g_variant_iter_free(address_tuples);
    
    return addresses;
}

void network_manager_free_addresses (ip4_config ** addresses)
{
    ip4_config ** address_pointer;

    for (address_pointer = addresses; *address_pointer != NULL; address_pointer++)
    {
        g_free(*address_pointer);
    }
    g_free(addresses);
}
