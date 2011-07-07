#include <check.h>
#include <gio/gio.h>
#include <glib.h>
#include <stdlib.h>

#include "../src/network-manager.h"


static GDBusConnection * connection;

static void setup ();
static void teardown ();


void setup ()
{
    connection = network_manager_init();
    fail_if(connection == NULL, "Init failed to connect.");
}

void teardown ()
{
    gboolean closed = g_dbus_connection_close_sync(connection, NULL, NULL);
    fail_unless(closed == TRUE, "Failed to close the DBus connection.");
}

START_TEST(test_get_devices)
{
    GVariant * devices = network_manager_get_devices(connection);

    if (! g_variant_is_of_type(devices, G_VARIANT_TYPE("(ao)")))
    {
        fail("Invalid type for device list.");
    }
    fail_unless(g_variant_n_children(devices) > 0, "Really? No devices?!");
}
END_TEST


TCase * network_manager_core_testcase ()
{
    TCase * testcase = tcase_create("Core");
    tcase_add_checked_fixture(testcase, setup, teardown);
    tcase_add_test(testcase, test_get_devices);

    return testcase;
}

Suite * network_manager_suite ()
{
    Suite * suite = suite_create("Network Manager");
    suite_add_tcase(suite, network_manager_core_testcase());

    return suite;
}
