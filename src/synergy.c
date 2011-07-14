#include <glib.h>
#include <stdlib.h>

#include "synergy.h"


gboolean synergy_connect (gchar * server)
{
    gchar * synergy_command;
    gboolean connected;

    g_debug("attempting to connect to synergy server '%s'", server);
    synergy_command = g_strconcat("synergyc ", server, NULL);
    g_debug("executing command '%s'", synergy_command);
    connected = system(synergy_command) == 0;
    g_free(synergy_command);

    return connected;
}

gboolean synergy_disconnect ()
{
    gboolean disconnected;

    g_debug("killing all synergyc processes");
    disconnected = system("killall synergyc") == 0;

    return disconnected;
}
