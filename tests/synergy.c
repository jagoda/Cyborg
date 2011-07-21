#include <glib.h>

#include "test_module.h"
#include "../src/synergy.h"


/*
   This test depends on external command masking to avoid system side effects.
*/


/* Test cases. */
static void test_connect ();
static void test_disconnect ();


void register_tests ()
{
    register_test(test_connect);
    register_test(test_disconnect);
}


void test_connect ()
{
    g_assert(synergy_connect("fake.synergy.server.com"));
}

void test_disconnect ()
{
    g_assert(synergy_disconnect());
}
