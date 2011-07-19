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
    }
    g_object_unref(system_bus);

    return device_state;
}


GDBusConnection * network_manager_connect ()
{
    GDBusConnection * connection;
    GError * error;

    error = NULL;
    connection = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);
    if (error)
    {
        g_error("Failed to connect to system bus: %s", error->message);
        g_error_free(error);
        error = NULL;
    }

    return connection;
}
