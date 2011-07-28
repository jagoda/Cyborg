#include <glib.h>

#include "test_module.h"
#include "../src/assimilator.h"
#include "../src/configuration_parser.h"
#include "../src/pulseaudio.h"


/* Test cases. */
static void test_connect_disconnect ();


static server_configuration * synthesize_configuration ();


void register_tests ()
{
    register_test(test_connect_disconnect);
}


void test_connect_disconnect ()
{
    server_configuration * configuration = NULL;
    gint tunnel_module_index = -1, loopback_module_index = -1;

    configuration = synthesize_configuration();
    g_assert(pulseaudio_connect(
                configuration,
                &tunnel_module_index,
                &loopback_module_index
            ));
    g_assert(pulseaudio_disconnect(
                tunnel_module_index,
                loopback_module_index
            ));

    configuration_parser_free_configuration(configuration);
}


server_configuration * synthesize_configuration ()
{
    server_configuration * configuration = NULL;

    configuration = (server_configuration *) g_malloc(
            sizeof(server_configuration)
        );
    configuration->server = g_strdup("bump.dyn.webahead.ibm.com");
    configuration->device_configuration = NULL;
    configuration->audio_configuration = (pulseaudio_config *) g_malloc(
            sizeof(pulseaudio_config)
        );
    configuration->audio_configuration->mode = PULSEAUDIO_MODE_PULL;
    configuration->audio_configuration->source = 0;
    configuration->audio_configuration->sink = 1;

    return configuration;
}
