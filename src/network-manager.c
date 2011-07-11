#include <gio/gio.h>
#include <glib.h>
#include <glib-object.h>
#include <stdlib.h>

#include "network-manager.h"


static const char * SERVICE = "org.freedesktop.NetworkManager";

static const char * MANAGER_OBJECT = "/org/freedesktop/NetworkManager";

static const char * MANAGER_INTERFACE = "org.freedesktop.NetworkManager";


GDBusConnection * network_manager_init ()
{
    GDBusConnection * connection;

    g_type_init();
    /* Connect to the DBus system bus. */
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

    /* Get available devices from DBus. */
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

    /* Extract device paths from the response. */
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
    /* FIXME: implement */
}
