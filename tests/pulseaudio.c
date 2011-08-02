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
static void test_pull ();
static void test_push ();


static void test_configuration (guint index);


void register_tests ()
{
    register_test(test_pull);
    register_test(test_push);
}


void test_pull ()
{
    test_configuration(1);
}

void test_push ()
{
    test_configuration(2);
}


void test_configuration (guint index)
{
    server_configuration ** configurations = NULL;
    GQueue * loaded_modules = NULL;

    if (! (configurations = configuration_parser_load(TEST_CONFIGURATION)))
    {
        g_error("Failed to load configuration file.");
    }
    loaded_modules = g_queue_new();
    g_assert(pulseaudio_connect(configurations[index], loaded_modules));
    g_assert(pulseaudio_disconnect(loaded_modules));

    g_queue_free(loaded_modules);
    configuration_parser_free_configurations(configurations);
}
