#include <glib.h>
#include <glib-object.h>
#include <stdlib.h>

#include "assimilator.h"


#define CONFIG_FILE     ".cyborg.conf"


int main (int argc, char ** argv)
{
    gchar * configuration_file;
    gboolean connected;

    g_type_init();

    configuration_file = g_strconcat(getenv("HOME"), "/", CONFIG_FILE, NULL);
    connected = assimilator_connect(configuration_file);
    g_free(configuration_file);

    return connected == TRUE ? EXIT_SUCCESS : EXIT_FAILURE;
}
