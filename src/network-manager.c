#include <gio/gio.h>
#include <glib-object.h>
#include <stdlib.h>

#include "network-manager.h"


static const char * SERVICE = "org.freedesktop.NetworkManager";

static const char * MANAGER_OBJECT = "/org/freedesktop/NetworkManager";

static const char * MANAGER_INTERFACE = "org.freedesktop.NetworkManager";


GDBusConnection * network_manager_init ()
{
    GDBusConnection * connection = NULL;

    g_type_init();
    connection = g_bus_get_sync(
            G_BUS_TYPE_SYSTEM,
            NULL,
            NULL
        );

    return connection;
}

GVariant * network_manager_get_devices (GDBusConnection * connection)
{
    GVariant * devices = g_dbus_connection_call_sync(
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

    return devices;
}
