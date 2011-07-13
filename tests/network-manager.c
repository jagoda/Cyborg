#include <check.h>
#include <gio/gio.h>
#include <glib.h>
#include <stdlib.h>
#include <string.h>

#include "../src/network.h"
#include "network-manager.h"


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
    gchar ** devices, ** path_pointer;
    gchar * device_path;
   
    devices = network_manager_get_devices(connection);
    fail_if(devices == NULL || *devices == NULL, "Really? No devices?!");

    g_debug("checking device list");
    for (path_pointer = devices; *path_pointer != NULL; path_pointer++)
    {
        device_path = *path_pointer;
        g_debug("checking device '%s'", device_path);
        fail_unless(strlen(device_path) > 0, "Empty device path.");
        fail_unless(device_path[0] == '/', "Invalid device path.");
    }
    network_manager_free_devices(devices);
}
END_TEST

START_TEST(test_device_state)
{
    gchar ** devices;
    gint state;

    devices = network_manager_get_devices(connection);
    state = network_manager_device_state(
            connection,
            *devices
        );
    fail_unless(state > NM_DEVICE_STATE_UNKNOWN, "Device state is unknown.");
    fail_unless(state <= NM_DEVICE_STATE_FAILED, "Invalid device state.");

    network_manager_free_devices(devices);
}
END_TEST

START_TEST(test_device_addresses)
{
    gchar ** devices, ** device_pointer, * device;
    gboolean device_tested;
    ip4_config ** addresses, * config;

    device_tested = FALSE;
    devices = network_manager_get_devices(connection);
    for (device_pointer = devices; *device_pointer != NULL; device_pointer++)
    {
        device = *device_pointer;
        if (network_manager_device_state(connection, device)
                == NM_DEVICE_STATE_ACTIVATED)
        {
            g_debug("getting address list for '%s'", device);
            addresses = network_manager_device_addresses(
                    connection,
                    device
                );

            g_debug("checking ip config for '%s'", device);
            config = *addresses;
            fail_if(config->address == 0, "Invalid address.");
            fail_if(config->prefix == 0, "Invalid prefix.");
            fail_if(config->gateway == 0, "Invalid gateway.");
            device_tested = TRUE;

            network_manager_free_addresses(addresses);
        }
    }
    network_manager_free_devices(devices);

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
