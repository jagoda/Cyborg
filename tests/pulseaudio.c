#include <glib.h>

#include "test_module.h"
#include "../src/assimilator.h"
#include "../src/configuration_parser.h"
#include "../src/pulseaudio.h"


/* 
   NOTE: this test cases uses settings found in the test configuration file
   to connect to a *real* pulseaudio server (those settings should be
   updated in order for the test cases to pass. All changes to the system
   should be undone if the tests pass but some residual changes could be
   left behind in the event of a test failure. You have been warned...
*/


#define TEST_CONFIGURATION      "test.conf"


/* Test cases. */
static void test_connect_disconnect ();


void register_tests ()
{
    register_test(test_connect_disconnect);
}


void test_connect_disconnect ()
{
    server_configuration ** configurations = NULL;
    gint tunnel_module_index = -1, loopback_module_index = -1;

    if (! (configurations = configuration_parser_load(TEST_CONFIGURATION)))
    {
        g_error("Failed to load configuration file.");
    }
    g_assert(pulseaudio_connect(
                configurations[1],
                &tunnel_module_index,
                &loopback_module_index
            ));
    g_assert(pulseaudio_disconnect(
                tunnel_module_index,
                loopback_module_index
            ));

    configuration_parser_free_configurations(configurations);
}
