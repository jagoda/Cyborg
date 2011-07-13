#include <check.h>

#include "../src/network.h"
#include "network.h"


static GDBusConnection * connection;

static void setup ();
static void teardown ();


void setup ()
{
    g_debug("preparing for test run");
    connection = network_manager_init();
    fail_if(connection == NULL, "failed to connect to DBus");
}

void teardown ()
{
    gboolean closed;

    g_debug("cleaning up test run");
    closed = g_dbus_connection_close_sync(connection, NULL, NULL);
    fail_unless(closed == TRUE, "Failed to close the DBus connection");
}

START_TEST(test_my_addresses)
{
    ip4_config ** addresses, ** address_pointer;
    gint address_count;

    addresses = network_my_addresses(connection);
    fail_if(addresses == NULL, "network_my_addresses returned NULL.");
    for (
            address_count = 0, address_pointer = addresses;
            *address_pointer != NULL;
            address_pointer++, address_count++
        );
    fail_unless(address_count > 0, "Really? No local addresses?");
    network_free_addresses(addresses);
}
END_TEST


TCase * network_core_testcase ()
{
    TCase * testcase;

    testcase = tcase_create("Core");
    tcase_add_checked_fixture(testcase, setup, teardown);
    tcase_add_test(testcase, test_my_addresses);

    return testcase;
}

Suite * network_suite ()
{
    Suite * suite;

    suite = suite_create("Network");
    suite_add_tcase(suite, network_core_testcase());

    return suite;
}
