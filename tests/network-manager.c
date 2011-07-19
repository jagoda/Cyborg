#include <glib.h>
#include <stdlib.h>
#include <string.h>

#include "test_module.h"
#include "../src/network-manager.h"


/* Test cases. */
void test_get_devices ();
void test_device_state ();


void register_tests ()
{
    register_test(test_get_devices);
    register_test(test_device_state);
}


void test_get_devices ()
{
    gchar ** devices, ** device_pointer;
    guint device_count;

    devices = network_manager_get_devices();
    g_assert(devices);
    for (
            device_count = 0, device_pointer = devices;
            *device_pointer != NULL;
            device_pointer++, device_count++
        )
    {
        g_assert(strlen(*device_pointer) > 0);
        g_assert((*device_pointer)[0] == '/');
    }
    g_assert(device_count > 0);
    network_manager_free_devices(devices);
}

void test_device_state ()
{
    gchar ** devices;
    guint32 device_state;

    devices = network_manager_get_devices();
    g_assert(devices);
    g_assert(*devices);

    device_state = network_manager_get_device_state(*devices);
    g_assert(device_state > NM_DEVICE_STATE_UNKNOWN);
    g_assert(device_state < NM_DEVICE_STATE_FAILED);
}
