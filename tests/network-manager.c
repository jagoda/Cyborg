#include <check.h>
#include <gio/gio.h>
#include <glib.h>
#include <stdlib.h>
#include <string.h>

#include "../src/network-manager.h"


static GDBusConnection * connection;

static void setup ();
static void teardown ();


void setup ()
{
    g_debug("preparing for test run");
    connection = network_manager_init();
    fail_if(connection == NULL, "Init failed to connect.");
}

void teardown ()
{
    gboolean closed;
   
    g_debug("cleaning up test run");
    closed = g_dbus_connection_close_sync(connection, NULL, NULL);
    fail_unless(closed == TRUE, "Failed to close the DBus connection.");
}

START_TEST(test_get_devices)
{
    GPtrArray * devices;
    gint index;
    gchar * device_path;
   
    devices = network_manager_get_devices(connection);
    fail_unless(devices->len > 0, "Really? No devices?!");

    g_debug("checking device list");
    for (index = 0; index < devices->len; index++)
    {
        device_path = (gchar *) g_ptr_array_index(devices, index);
        g_debug("checking device '%s'", device_path);
        fail_unless(strlen(device_path) > 0, "Empty device path.");
        fail_unless(device_path[0] == '/', "Invalid device path.");
        g_free(device_path);
    }
    g_ptr_array_free(devices, TRUE);
}
END_TEST

START_TEST(test_device_state)
{
    GPtrArray * devices;
    gint index, state;

    devices = network_manager_get_devices(connection);
    state = network_manager_device_state(
            connection,
            (gchar *) g_ptr_array_index(devices, 0)
        );
    fail_unless(state > NM_DEVICE_STATE_UNKNOWN, "Device state is unknown.");
    fail_unless(state <= NM_DEVICE_STATE_FAILED, "Invalid device state.");

    for (index = 0; index < devices->len; index++)
    {
        g_free(g_ptr_array_index(devices, index));
    }
    g_ptr_array_free(devices, TRUE);
}
END_TEST

START_TEST(test_device_addresses)
{
    GPtrArray * devices, * addresses;
    gchar * device;
    gint index;
    gboolean device_tested;
    ip4_config * config;

    device_tested = FALSE;
    devices = network_manager_get_devices(connection);
    for (index = 0; index < devices->len; index++)
    {
        device = (gchar *) g_ptr_array_index(devices, index);
        if (network_manager_device_state(connection, device)
                == NM_DEVICE_STATE_ACTIVATED)
        {
            g_debug("getting address list for '%s'", device);
            addresses = network_manager_device_addresses(
                    connection,
                    device
                );

            g_debug("checking ip config for '%s'", device);
            config = (ip4_config *) g_ptr_array_index(addresses, 0);
            fail_if(config->address == 0, "Invalid address.");
            fail_if(config->prefix == 0, "Invalid prefix.");
            fail_if(config->gateway == 0, "Invalid gateway.");
            device_tested = TRUE;
        }

        g_free(device);
    }
    g_ptr_array_free(devices, TRUE);

    fail_unless(device_tested, "No devices available for address test.");
}
END_TEST


TCase * network_manager_core_testcase ()
{
    TCase * testcase;
   
    testcase = tcase_create("Core");
    tcase_add_checked_fixture(testcase, setup, teardown);
    tcase_add_test(testcase, test_get_devices);
    tcase_add_test(testcase, test_device_state);
    tcase_add_test(testcase, test_device_addresses);

    return testcase;
}

Suite * network_manager_suite ()
{
    Suite * suite;
   
    suite = suite_create("Network Manager");
    suite_add_tcase(suite, network_manager_core_testcase());

    return suite;
}
