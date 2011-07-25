#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <glib.h>


typedef enum {
    NM_DEVICE_STATE_UNKNOWN,
    NM_DEVICE_STATE_UNMANAGED,
    NM_DEVICE_STATE_UNAVAILABLE,
    NM_DEVICE_STATE_DISCONNECTED,
    NM_DEVICE_STATE_PREPARE,
    NM_DEVICE_STATE_CONFIG,
    NM_DEVICE_STATE_NEED_AUTH,
    NM_DEVICE_STATE_IP_CONFIG,
    NM_DEVICE_STATE_ACTIVATED,
    NM_DEVICE_STATE_FAILED
} network_manager_device_state;

typedef struct {
    guint32 ip_address;
    guint32 prefix;
    guint32 gateway_address;
} network_manager_ip4config;


gchar ** network_manager_get_devices ();

void network_manager_free_devices (gchar ** devices);

guint32 network_manager_get_device_state (gchar * device);

gchar * network_manager_get_ip4config (gchar * device);

network_manager_ip4config ** network_manager_get_addresses (
        gchar * ip4config
    );

network_manager_ip4config ** network_manager_all_addresses ();

void network_manager_free_addresses (network_manager_ip4config ** addresses);

guint network_manager_register_connect_handler (
        void (*handler)(guint state)
    );

#endif /* NETWORK_MANAGER_H */
