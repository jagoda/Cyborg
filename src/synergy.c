#include <glib.h>
#include <stdlib.h>

#include "synergy.h"


gboolean synergy_connect (gchar * server)
{
    gchar * connect_command;
    guint exit_code;

    connect_command = g_strconcat("synergyc ", server, NULL);
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
