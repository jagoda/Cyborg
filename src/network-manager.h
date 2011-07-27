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

typedef struct {
    gchar * device_name;
    network_manager_ip4config ** ip_config;
} network_manager_device_config;


gchar ** network_manager_get_devices ();

void network_manager_free_devices (gchar ** devices);

guint32 network_manager_get_device_state (gchar * device);

gchar * network_manager_get_device_name (gchar * device);

gchar * network_manager_get_ip4config (gchar * device);

network_manager_ip4config ** network_manager_get_addresses (
        gchar * device
    );

void network_manager_free_addresses (network_manager_ip4config ** addresses);

network_manager_device_config ** network_manager_device_configurations ();

void network_manager_free_device_configuration (
        network_manager_device_config * device_configuration
    );

void network_manager_free_device_configurations (
        network_manager_device_config ** configurations
    );

guint network_manager_register_connect_handler (
        void (*handler)(guint state)
    );

#endif /* NETWORK_MANAGER_H */
