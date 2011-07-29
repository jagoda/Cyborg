#include <glib.h>
#include <string.h>

#include "test_module.h"
#include "../src/configuration_parser.h"


#define TEST_CONFIGURATION      "test.conf"


/* Test cases. */
static void test_load ();


void register_tests ()
{
    register_test(test_load);
}


void test_load ()
{
    server_configuration ** configurations;
    server_configuration * configuration;
    guint configuration_count;

    configurations = configuration_parser_load(TEST_CONFIGURATION);
    g_assert(configurations);

    for (
            configuration_count = 0;
            configurations[configuration_count] != NULL;
            configuration_count++
        )
    {
        configuration = configurations[configuration_count];
        g_assert(configuration->server);
        g_assert_cmpuint(strlen(configuration->server), >, 0);
        g_assert(configuration->device_configuration);
        g_assert(configuration->device_configuration->device_name);
        g_assert_cmpuint(
                strlen(configuration->device_configuration->device_name),
                >,
                0
            );
        g_assert_cmpuint(
                PULSEAUDIO_MODE_PULL,
                ==,
                configuration->audio_configuration->mode
            );
    }
    g_assert_cmpuint(configuration_count, >, 0);

    configuration_parser_free_configuration(configuration);
}
