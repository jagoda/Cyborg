#ifndef PULSEAUDIO_H
#define PULSEAUDIO_H

#include <glib.h>

#include "types.h"


gboolean pulseaudio_connect (
        server_configuration * configuration,
        GQueue * loaded_modules
    );

gboolean pulseaudio_disconnect (GQueue * loaded_modules);

#endif      /* PULSEAUDIO_H */
