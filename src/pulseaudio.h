#ifndef PULSEAUDIO_H
#define PULSEAUDIO_H

#include <glib.h>


gboolean pulseaudio_connect (
        gchar * server,
        guint source_index,
        guint sink_index
    );

gboolean pulseaudio_disconnect ();

#endif      /* PULSEAUDIO_H */
