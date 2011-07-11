#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <gio/gio.h>
#include <glib.h>


#define NM_DEVICE_STATE_UNKNOWN         0
#define NM_DEVICE_STATE_UNMANAGED       1
#define NM_DEVICE_STATE_UNAVAILABLE     2
#define NM_DEVICE_STATE_DISCONNECTED    3
#define NM_DEVICE_STATE_PREPARE         4
#define NM_DEVICE_STATE_CONFIG          5
#define NM_DEVICE_STATE_NEED_AUTH       6
#define NM_DEVICE_STATE_IP_CONFIG       7
#define NM_DEVICE_STATE_ACTIVATED       8
#define NM_DEVICE_STATE_FAILED          9


GDBusConnection * network_manager_init ();

GPtrArray * network_manager_get_devices (GDBusConnection * connection);

gint network_manager_device_state (
        GDBusConnection * connection,
        gchar * device
    );

#endif /* NETWORK_MANAGER_H */
