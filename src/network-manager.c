#include <gio/gio.h>
#include <glib.h>
#include <glib-object.h>
#include <stdlib.h>

#include "network-manager.h"


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

    g_debug("initializing network manager module");
    g_type_init();
    g_debug("connecting to the system bus");
    connection = g_bus_get_sync(
            G_BUS_TYPE_SYSTEM,
            NULL,
            NULL
        );

    return connection;
}

GPtrArray * network_manager_get_devices (GDBusConnection * connection)
{
    GVariant * dbus_response;
    GPtrArray * devices;
    GVariantIter * object_paths;
    gchar * path;

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
    devices = g_ptr_array_new();
    g_variant_get(dbus_response, "(ao)", &object_paths);
    while (g_variant_iter_loop(object_paths, "o", &path))
    {
        g_ptr_array_add(devices, g_strdup(path));
    }
    g_variant_iter_free(object_paths);
    g_variant_unref(dbus_response);

    return devices;
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

GPtrArray * network_manager_device_addresses (
        GDBusConnection * connection,
        gchar * device
    )
{
    GVariant * dbus_parameters, * dbus_response, * response_payload;
    GPtrArray * addresses;
    const gchar * config_path;
    GVariantIter * address_tuples, * address_parts;
    ip4_config * ip_config;

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
    addresses = g_ptr_array_new();
    while (g_variant_iter_loop(address_tuples, "au", &address_parts))
    {
        ip_config = (ip4_config *) malloc(sizeof(ip4_config));
        g_variant_iter_next(address_parts, "u", &ip_config->address);
        g_variant_iter_next(address_parts, "u", &ip_config->prefix);
        g_variant_iter_next(address_parts, "u", &ip_config->gateway);
        g_ptr_array_add(addresses, ip_config);
    }
    g_variant_iter_free(address_tuples);
    
    return addresses;
}
