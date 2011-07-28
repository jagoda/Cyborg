#ifndef TYPES_H
#define TYPES_H

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

typedef enum {
    PULSEAUDIO_MODE_PULL,
    PULSEAUDIO_MODE_PUSH
} pulseaudio_mode;


typedef struct {
    guint32 ip_address;
    guint32 prefix;
    guint32 gateway_address;
} network_manager_ip4config;

typedef struct {
    gchar * device_name;
    network_manager_ip4config ** ip_config;
} network_manager_device_config;

typedef struct {
    pulseaudio_mode mode;
    guint source;
    guint sink;
} pulseaudio_config;

typedef struct {
    gchar * server;
    network_manager_device_config * device_configuration;
    pulseaudio_config * audio_configuration;
} server_configuration;

#endif  /* TYPES_H */
