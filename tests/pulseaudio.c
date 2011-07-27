#include <glib.h>

#include "test_module.h"
#include "../src/pulseaudio.h"


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
    /* FIXME: need to load parameters from configuration ??? */
    g_assert(pulseaudio_connect("bump.dyn.webahead.ibm.com", 1, 0));
}

void test_disconnect ()
{
    g_assert(pulseaudio_disconnect());
}
