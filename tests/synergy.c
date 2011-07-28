#include <glib.h>

#include "test_module.h"
#include "../src/assimilator.h"
#include "../src/synergy.h"


/*
   This test depends on external command masking to avoid system side effects.
*/


/* Test cases. */
static void test_connect ();
static void test_disconnect ();


static server_configuration * synthesize_configuration ();


void register_tests ()
{
    register_test(test_connect);
    register_test(test_disconnect);
}


void test_connect ()
{
    g_assert(synergy_connect(synthesize_configuration()));
}

void test_disconnect ()
{
    g_assert(synergy_disconnect());
}


server_configuration * synthesize_configuration ()
{
    server_configuration * configuration = NULL;

    configuration = (server_configuration *) g_malloc(
            sizeof(server_configuration)
        );
    configuration->server = g_strdup("fake.synergy.server.com");
    configuration->device_configuration = NULL;

    return configuration;
}
