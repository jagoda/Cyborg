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


gchar ** network_manager_get_devices ();

void network_manager_free_devices (gchar ** devices);

guint32 network_manager_get_device_state (gchar * device);

#endif /* NETWORK_MANAGER_H */
