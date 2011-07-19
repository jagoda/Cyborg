#include <glib.h>

#include "test_api.h"


/* Test cases. */
static void test_foo ();


void register_tests ()
{
    register_test(test_foo);
    /* TODO: implement */
}


void test_foo ()
{
    g_assert(TRUE);
}
