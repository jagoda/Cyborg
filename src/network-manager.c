#include <arpa/inet.h>
#include <glib.h>
#include <gio/gio.h>
#include <stdlib.h>

#include "network-manager.h"


#define DBUS_TIMEOUT    -1


static const gchar * NETWORK_MANAGER_SERVICE =
    "org.freedesktop.NetworkManager";
static const gchar * NETWORK_MANAGER_OBJECT =
    "/org/freedesktop/NetworkManager";
static const gchar * NETWORK_MANAGER_INTERFACE =
    "org.freedesktop.NetworkManager";
static const gchar * NETWORK_MANAGER_GET_DEVICES = "GetDevices";

static const gchar * NETWORK_MANAGER_DEVICE_INTERFACE =
    "org.freedesktop.NetworkManager.Device";
static const gchar * NETWORK_MANAGER_DEVICE_STATE = "State";
static const gchar * NETWORK_MANAGER_DEVICE_IP4CONFIG = "Ip4Config";

static const gchar * NETWORK_MANAGER_IP4CONFIG_INTERFACE =
    "org.freedesktop.NetworkManager.IP4Config";
static const gchar * NETWORK_MANAGER_IP4CONFIG_ADDRESSES = "Addresses";

static const gchar * DBUS_PROPERTIES_INTERFACE =
    "org.freedesktop.DBus.Properties";
static const gchar * DBUS_PROPERTIES_GET = "Get";


static GDBusConnection * network_manager_connect ();


gchar ** network_manager_get_devices ()
{
    GDBusConnection * system_bus;
    GVariant * dbus_response;
    GVariantIter * path_iterator;
    GError * error;
    gchar ** devices, ** device_pointer, * object_path;
    gsize path_count;

    devices = NULL;
    if ((system_bus = network_manager_connect()))
    {
        error = NULL;
        if ((
                dbus_response = g_dbus_connection_call_sync(
                    system_bus,
                    NETWORK_MANAGER_SERVICE,
                    NETWORK_MANAGER_OBJECT,
                    NETWORK_MANAGER_INTERFACE,
                    NETWORK_MANAGER_GET_DEVICES,
                    NULL,
                    G_VARIANT_TYPE("(ao)"),
                    G_DBUS_CALL_FLAGS_NONE,
                    DBUS_TIMEOUT,
                    NULL,
                    &error
                )
            ))
        {
            g_variant_get(dbus_response, "(ao)", &path_iterator);
            path_count = g_variant_iter_n_children(path_iterator);
            devices = (gchar **) g_malloc(
                    (path_count + 1) * sizeof(gchar *)
                );
            for (
                    device_pointer = devices;
                    g_variant_iter_loop(path_iterator, "o", &object_path);
                    device_pointer++
                )
            {
                *device_pointer = g_strdup(object_path);
            }
            *device_pointer = NULL;

            g_variant_unref(dbus_response);
            g_variant_iter_free(path_iterator);
        }
        else
        {
            g_error(
                    "Call to '%s' failed: %s.",
                    NETWORK_MANAGER_GET_DEVICES,
                    error->message
                );
            g_error_free(error);
            error = NULL;
        }
    }
    g_object_unref(system_bus);

    return devices;
}

void network_manager_free_devices (gchar ** devices)
{
    gchar ** device_pointer;

    if (devices)
    {
        for (
                device_pointer = devices;
                *device_pointer != NULL;
                device_pointer++
            )
        {
            g_free(*device_pointer);
        }
        g_free(devices);
    }
}

guint32 network_manager_get_device_state (gchar * device)
{
    GDBusConnection * system_bus;
    GVariant * dbus_response, * dbus_parameters, * response_payload;
    GError * error;
    guint32 device_state;

    if (! device)
    {
        g_error("Device argument cannot be NULL.");
    }
    device_state = 0;
    if ((system_bus = network_manager_connect()))
    {
        error = NULL;
        dbus_parameters = g_variant_new(
                "(ss)",
                NETWORK_MANAGER_DEVICE_INTERFACE,
                NETWORK_MANAGER_DEVICE_STATE
            );
        if ((dbus_response = g_dbus_connection_call_sync(
                        system_bus,
                        NETWORK_MANAGER_SERVICE,
                        device,
                        DBUS_PROPERTIES_INTERFACE,
                        DBUS_PROPERTIES_GET,
                        dbus_parameters,
                        G_VARIANT_TYPE("(v)"),
                        G_DBUS_CALL_FLAGS_NONE,
                        DBUS_TIMEOUT,
                        NULL,
                        &error
                    )
            ))
        {
            g_variant_get(dbus_response, "(v)", &response_payload);
            device_state = g_variant_get_uint32(response_payload);
            g_variant_unref(response_payload);
            g_variant_unref(dbus_response);
        }
        else
        {
            g_error(
                    "Failed to get device state from DBus: %s",
                    error->message
                );
            g_error_free(error);
            error = NULL;
        }
        g_variant_unref(dbus_parameters);
        g_object_unref(system_bus);
    }

    return device_state;
}

gchar * network_manager_get_ip4config (gchar * device)
{
    GDBusConnection * system_bus;
    GVariant * dbus_response, *dbus_parameters, * response_payload;
    GError * error;
    gchar * ip4config_path;

    if (device == NULL)
    {
        g_error("Network device path cannot be NULL.");
    }
    ip4config_path = NULL;
    if ((system_bus = network_manager_connect()))
    {
        dbus_parameters = g_variant_new(
                "(ss)",
                NETWORK_MANAGER_DEVICE_INTERFACE,
                NETWORK_MANAGER_DEVICE_IP4CONFIG
            );
        error = NULL;
        if ((dbus_response = g_dbus_connection_call_sync(
                        system_bus,
                        NETWORK_MANAGER_SERVICE,
                        device,
                        DBUS_PROPERTIES_INTERFACE,
                        DBUS_PROPERTIES_GET,
                        dbus_parameters,
                        G_VARIANT_TYPE("(v)"),
                        G_DBUS_CALL_FLAGS_NONE,
                        DBUS_TIMEOUT,
                        NULL,
                        &error
                    )
            ))
        {
            g_variant_get(dbus_response, "(v)", &response_payload);
            ip4config_path = g_variant_dup_string(response_payload, NULL);
            g_variant_unref(response_payload);
            g_variant_unref(dbus_response);
        }
        else
        {
            g_error(
                    "Failed to get Ip4Config for device: %s.",
                    error->message
                );
            g_error_free(error);
            error = NULL;
        }
        g_variant_unref(dbus_parameters);

        g_object_unref(system_bus);
    }

    return ip4config_path;
}

network_manager_ip4config ** network_manager_get_addresses (
        gchar * ip4config
    )
{
    GDBusConnection * system_bus;
    GVariant * dbus_response, * dbus_parameters, * response_payload;
    GVariantIter * addresses, * address_parts;
    GError * error;
    network_manager_ip4config ** ip_configs, ** config_pointer;
    network_manager_ip4config * ip_config;
    guint address_count;

    if (ip4config == NULL)
    {
        g_error("Ip4Config path cannot be NULL.");
    }
    ip_configs = NULL;
    if ((system_bus = network_manager_connect()))
    {
        dbus_parameters = g_variant_new(
                "(ss)",
                NETWORK_MANAGER_IP4CONFIG_INTERFACE,
                NETWORK_MANAGER_IP4CONFIG_ADDRESSES
            );
        if ((dbus_response = g_dbus_connection_call_sync(
                        system_bus,
                        NETWORK_MANAGER_SERVICE,
                        ip4config,
                        DBUS_PROPERTIES_INTERFACE,
                        DBUS_PROPERTIES_GET,
                        dbus_parameters,
                        G_VARIANT_TYPE("(v)"),
                        G_DBUS_CALL_FLAGS_NONE,
                        DBUS_TIMEOUT,
                        NULL,
                        &error
                    )
            ))
        {
            g_variant_get(dbus_response, "(v)", &response_payload);
            g_variant_unref(dbus_response);
            g_variant_get(response_payload, "aau", &addresses);
            g_variant_unref(response_payload);

            address_count = g_variant_iter_n_children(addresses);
            ip_configs = (network_manager_ip4config **) g_malloc(
                    (address_count + 1) * sizeof(network_manager_ip4config *)
                );
            for (
                    config_pointer = ip_configs;
                    g_variant_iter_loop(addresses, "au", &address_parts);
                    config_pointer++
                )
            {
                ip_config = *config_pointer =
                    (network_manager_ip4config *) g_malloc(
                            sizeof(network_manager_ip4config)
                        );
                g_variant_iter_next(
                        address_parts,
                        "u",
                        &(ip_config->ip_address)
                    );
                g_variant_iter_next(
                        address_parts,
                        "u",
                        &(ip_config->prefix)
                    );
                g_variant_iter_next(
                        address_parts,
                        "u",
                        &(ip_config->gateway_address)
                    );

                ip_config->ip_address = ntohl(ip_config->ip_address);
                ip_config->gateway_address = ntohl(ip_config->gateway_address);
            }
            *config_pointer = NULL;

            g_variant_iter_free(addresses);
        }
        else
        {
            g_error(
                    "Failed to get addresses for device: %s",
                    error->message
                );
            g_error_free(error);
            error = NULL;
        }
        g_variant_unref(dbus_parameters);

        g_object_unref(system_bus);
    }

    return ip_configs;
}

network_manager_ip4config ** network_manager_all_addresses ()
{
    GPtrArray * buffer;
    network_manager_ip4config ** ip_configs;
    network_manager_ip4config ** device_configs, ** config_pointer;
    gchar ** devices, ** device_pointer, * ip4config_path;
    guint index;

    ip_configs = NULL;
    devices = network_manager_get_devices();
    if (devices)
    {
        buffer = g_ptr_array_new();
        for (
                device_pointer = devices;
                *device_pointer != NULL;
                device_pointer++
            )
        {
            if (
                    network_manager_get_device_state(*device_pointer)
                    ==
                    NM_DEVICE_STATE_ACTIVATED
                )
            {
                if (! (
                        ip4config_path
                        =
                        network_manager_get_ip4config(*device_pointer)
                    ))
                {
                    g_error("Failed to get Ip4Config path for device.");
                }
                device_configs =
                    network_manager_get_addresses(ip4config_path);
                g_free(ip4config_path);
                if (! device_configs)
                {
                    g_error("No configuration returned for device.");
                }
                for (
                        config_pointer = device_configs;
                        *config_pointer != NULL;
                        config_pointer++
                    )
                {
                    g_ptr_array_add(buffer, *config_pointer);
                }

                /* No longer need array for pointers, but still need
                   allocated data itself. */
                g_free(device_configs);
            }
        }

        ip_configs = (network_manager_ip4config **) g_malloc (
                (buffer->len + 1) * sizeof(network_manager_ip4config *)
            );
        for (index = 0; index < buffer->len; index++)
        {
            ip_configs[index] = g_ptr_array_index(buffer, index);
        }
        ip_configs[index] = NULL;
        g_ptr_array_free(buffer, TRUE);
        network_manager_free_devices(devices);
    }

    return ip_configs;
}

void network_manager_free_addresses (network_manager_ip4config ** addresses)
{
    network_manager_ip4config ** address_pointer;

    if (addresses)
    {
        for (
                address_pointer = addresses;
                *address_pointer != NULL;
                address_pointer++
            )
        {
            g_free(*address_pointer);
        }
        g_free(addresses);
    }
}


GDBusConnection * network_manager_connect ()
{
    GDBusConnection * connection;
    GError * error;

    error = NULL;
    connection = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);
    if (error)
    { g_error("Failed to connect to system bus: %s", error->message);
        g_error_free(error);
        error = NULL;
    }

    return connection;
}
