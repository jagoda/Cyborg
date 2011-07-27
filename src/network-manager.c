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
static const gchar * NETWORK_MANAGER_DEVICE_NAME = "Interface";
static const gchar * NETWORK_MANAGER_DEVICE_IP4CONFIG = "Ip4Config";
static const gchar * NETWORK_MANAGER_DEVICE_STATE_CHANGED = "StateChanged";

static const gchar * NETWORK_MANAGER_IP4CONFIG_INTERFACE =
    "org.freedesktop.NetworkManager.IP4Config";
static const gchar * NETWORK_MANAGER_IP4CONFIG_ADDRESSES = "Addresses";

static const gchar * DBUS_PROPERTIES_INTERFACE =
    "org.freedesktop.DBus.Properties";
static const gchar * DBUS_PROPERTIES_GET = "Get";


static GDBusConnection * network_manager_connect ();

static guint subscribe_to_signal (
        void (*handler)(guint state),
        gchar * signal
    );

static void signal_callback_wrapper (
        GDBusConnection * connection,
        const gchar * sender_name,
        const gchar * object_path,
        const gchar * interface_name,
        const gchar * signal_name,
        GVariant * parameters,
        gpointer user_data
    );


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

gchar * network_manager_get_device_name (gchar * device)
{
    GDBusConnection * system_bus;
    GVariant * dbus_response, * dbus_parameters, * response_payload;
    GError * error;
    gchar * device_name;

    if (! device)
    {
        g_error("Device path cannot be NULL.");
    }

    device_name = NULL;
    if (( system_bus = network_manager_connect() ))
    {
        dbus_parameters = g_variant_new(
                "(ss)",
                NETWORK_MANAGER_DEVICE_INTERFACE,
                NETWORK_MANAGER_DEVICE_NAME
            );
        error = NULL;
        if ((
                dbus_response = g_dbus_connection_call_sync(
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
                    &error)
            ))
        {
            g_variant_get(dbus_response, "(v)", &response_payload);
            device_name = g_variant_dup_string(response_payload, NULL);
            g_variant_unref(response_payload);
            g_variant_unref(dbus_response);
        }
        else
        {
            g_error("Failed to get device name from DBus: %s", error->message);
        }
        g_variant_unref(dbus_parameters);
    }

    return device_name;
}

gchar * network_manager_get_ip4config (gchar * device)
{
    GDBusConnection * system_bus;
    GVariant * dbus_response, * dbus_parameters, * response_payload;
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
        gchar * device
    )
{
    GDBusConnection * system_bus;
    GVariant * dbus_response, * dbus_parameters, * response_payload;
    GVariantIter * addresses, * address_parts;
    GError * error;
    gchar * ip4config;
    network_manager_ip4config ** ip_configs, ** config_pointer;
    network_manager_ip4config * ip_config;
    guint address_count;

    if (device == NULL)
    {
        g_error("Device path cannot be NULL.");
    }

    if (!(ip4config = network_manager_get_ip4config(device)))
    {
        g_error("Failed to get IP4Config path for device.");
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

network_manager_device_config ** network_manager_device_configurations ()
{
    network_manager_device_config ** device_configurations;
    network_manager_device_config * configuration;
    gchar ** device_paths, ** path_pointer;
    GPtrArray * active_devices;
    guint index;

    device_configurations = NULL;
    active_devices = g_ptr_array_new();
    device_paths = network_manager_get_devices();

    for (
            path_pointer = device_paths;
            *path_pointer != NULL;
            path_pointer++
        )
    {
        if (
                network_manager_get_device_state(*path_pointer)
                ==
                NM_DEVICE_STATE_ACTIVATED
            )
        {
            configuration = (network_manager_device_config *) g_malloc(
                    sizeof(network_manager_device_config)
                );
            configuration->device_name =
                network_manager_get_device_name(*path_pointer);
            configuration->ip_config =
                network_manager_get_addresses(*path_pointer);
            g_ptr_array_add(active_devices, configuration);
        }
    }
    network_manager_free_devices(device_paths);

    device_configurations = (network_manager_device_config **) g_malloc(
            (active_devices->len + 1) * sizeof(network_manager_device_config *)
        );
    for (index = 0; index < active_devices->len; index++)
    {
        device_configurations[index] = g_ptr_array_index(
                active_devices,
                index
            );
    }
    device_configurations[index] = NULL;
    g_ptr_array_unref(active_devices);

    return device_configurations;
}

void network_manager_free_device_configuration (
        network_manager_device_config * device_configuration
    )
{
    if (device_configuration)
    {
        g_free(device_configuration->device_name);
        network_manager_free_addresses(device_configuration->ip_config);
        g_free(device_configuration);
    }
}

void network_manager_free_device_configurations (
        network_manager_device_config ** configurations
    )
{
    network_manager_device_config ** configuration_pointer;

    if (configurations)
    {
        for (
                configuration_pointer = configurations;
                *configuration_pointer != NULL;
                configuration_pointer++
            )
        {
            network_manager_free_device_configuration(*configuration_pointer);
        }
        g_free(configurations);
    }
}

guint network_manager_register_connect_handler (
        void (*handler)(guint state)
    )
{
    return subscribe_to_signal(
            handler,
            (gchar *) NETWORK_MANAGER_DEVICE_STATE_CHANGED
        );
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

guint subscribe_to_signal (
        void (*handler)(guint state),
        gchar * signal
    )
{
    GDBusConnection * system_bus;
    guint subscription_id;

    subscription_id = 0;
    if (( system_bus = network_manager_connect() ))
    {
        subscription_id = g_dbus_connection_signal_subscribe(
                system_bus,
                NETWORK_MANAGER_SERVICE,
                NETWORK_MANAGER_DEVICE_INTERFACE,
                signal,
                NULL,
                NULL,
                G_DBUS_SIGNAL_FLAGS_NONE,
                signal_callback_wrapper,
                handler,
                NULL
            );
    }

    return subscription_id;
}

void signal_callback_wrapper (
        GDBusConnection * connection,
        const gchar * sender_name,
        const gchar * object_path,
        const gchar * interface_name,
        const gchar * signal_name,
        GVariant * parameters,
        gpointer user_data
    )
{
    guint old_state, new_state, reason;
    void (*handler)(guint state);

    handler = user_data;
    g_variant_get(parameters, "(uuu)", &new_state, &old_state, &reason);
    handler(new_state);
}
