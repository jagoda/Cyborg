#include <check.h>
#include <glib.h>
#include <glib-object.h>
#include <stdlib.h>

#include "../src/synergy.h"
#include "synergy.h"


/*
   This test depends on modifying the system path to point to a dummy Synergy
   binary (in order to simulate service execution). This dummy should also
   be located in the test directory.
*/


static gchar * environment_path;


static void setup ();
static void teardown ();


void setup ()
{
    gchar * updated_path;
    gint updated;

    /* FIXME: should this really go here? */
    g_type_init();

    g_debug("hacking the path to load a mock binary");
    environment_path = g_strdup(getenv("PATH"));
    updated_path = g_strconcat(".:", environment_path, NULL);
    updated = setenv("PATH", updated_path, TRUE);
    g_free(updated_path);
    fail_if(updated == -1, "failed to update system path");
}

void teardown ()
{
    gint updated;

    g_debug("undoing path hacks");
    updated = setenv("PATH", environment_path, TRUE);
    g_free(environment_path);
    fail_if(updated == -1, "failed to reset system path");
}

START_TEST(test_connect)
{
    gboolean connected;

    connected = synergy_connect("bump.dyn.webahead.ibm.com");
    fail_unless(connected, "failed to connected to simulated server");
}
END_TEST

START_TEST(test_disconnect)
{
    gboolean disconnected;

    disconnected = synergy_disconnect();
    fail_unless(disconnected, "failed to disconnect from the simulated server");
}
END_TEST


TCase * synergy_core_testcase ()
{
    TCase * testcase;
    
    testcase = tcase_create("Core");
    tcase_add_checked_fixture(testcase, setup, teardown);
    tcase_add_test(testcase, test_connect);
    tcase_add_test(testcase, test_disconnect);
    
    return testcase;
}

Suite * synergy_suite ()
{
    Suite * suite;

    suite = suite_create("Synergy");
    suite_add_tcase(suite, synergy_core_testcase());

    return suite;
}
