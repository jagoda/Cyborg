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
    server_configuration ** configurations, ** configuration_pointer;
    server_configuration * configuration;
    guint configuration_count;

    configurations = configuration_parser_load(TEST_CONFIGURATION);
    g_assert(configurations);

    for (
            configuration_pointer = configurations, configuration_count = 0;
            *configuration_pointer != NULL;
            configuration_pointer++, configuration_count++
        )
    {
        configuration = *configuration_pointer;
        g_assert(configuration->server);
        g_assert_cmpuint(strlen(configuration->server), >, 0);
        g_assert(configuration->network_config);
        g_assert_cmpuint( configuration->network_config->ip_address, >, 0);
    }
    g_assert_cmpuint(configuration_count, >, 0);
}
