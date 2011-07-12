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


typedef struct {
    guint32 address;
    guint32 prefix;
    guint32 gateway;
} ip4_config;


GDBusConnection * network_manager_init ();

gchar ** network_manager_get_devices (GDBusConnection * connection);

void network_manager_free_devices (gchar ** devices);

gint network_manager_device_state (
        GDBusConnection * connection,
        gchar * device
    );

ip4_config ** network_manager_device_addresses (
        GDBusConnection * connection,
        gchar * device
    );

void network_manager_free_addresses (ip4_config ** addresses);

#endif /* NETWORK_MANAGER_H */
