#ifndef PULSEAUDIO_H
#define PULSEAUDIO_H

#include <glib.h>

#include "types.h"


gboolean pulseaudio_connect (
        server_configuration * configuration,
        gint * tunnel_module_index,
        gint * loopback_module_index
    );

gboolean pulseaudio_disconnect (
        gint tunnel_module_index,
        gint loopback_module_index
    );

#endif      /* PULSEAUDIO_H */
