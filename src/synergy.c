#include <glib.h>
#include <stdlib.h>

#include "synergy.h"


gboolean synergy_connect (const server_configuration * configuration)
{
    gchar * connect_command;
    guint exit_code;

    if (! configuration)
    {
        g_error("Configuration pointer must not be NULL.");
    }

    connect_command = g_strconcat("synergyc ", configuration->server, NULL);
    exit_code = system(connect_command);
    g_free(connect_command);

    return exit_code == 0;
}

gboolean synergy_disconnect ()
{
    guint exit_code;

    exit_code = system("killall synergyc");

    return exit_code == 0;
}
