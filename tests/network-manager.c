#include <glib.h>
#include <stdlib.h>
#include <string.h>

#include "test_module.h"
#include "../src/network-manager.h"


/* Test cases. */
static void test_get_devices ();
static void test_device_state ();
static void test_device_name ();
static void test_ip4config ();
static void test_device_addresses ();
static void test_device_configurations ();


void register_tests ()
{
    register_test(test_get_devices);
    register_test(test_device_state);
    register_test(test_device_name);
    register_test(test_ip4config);
    register_test(test_device_addresses);
    register_test(test_device_configurations);
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
        g_assert_cmpint(strlen(*device_pointer), >, 0);
        g_assert((*device_pointer)[0] == '/');
    }
    g_assert_cmpuint(device_count, >, 0);
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
    g_assert_cmpuint(device_state, >, NM_DEVICE_STATE_UNKNOWN);
    g_assert_cmpuint(device_state, <, NM_DEVICE_STATE_FAILED);
}

void test_device_name ()
{
    gchar ** devices, * device_name;

    devices = network_manager_get_devices();
    g_assert(devices);
    g_assert(*devices);

    device_name = network_manager_get_device_name(*devices);
    g_assert(device_name);
    g_assert_cmpuint(strlen(device_name), >, 0);
}

void test_ip4config ()
{
    gchar ** devices, ** device_pointer, * ip4config_path;
    gboolean active_device_found;
    guint32 device_state;

    devices = network_manager_get_devices();
    g_assert(devices);

    active_device_found = FALSE;
    ip4config_path = FALSE;
    for (
            device_pointer = devices;
            *device_pointer != NULL;
            device_pointer++
        )
    {
        device_state = network_manager_get_device_state(*device_pointer);

        if (device_state == NM_DEVICE_STATE_ACTIVATED)
        {
            active_device_found = TRUE;
            ip4config_path = network_manager_get_ip4config(*device_pointer);
            g_assert(ip4config_path);
            g_assert_cmpuint(strlen(ip4config_path), >, 0);
            g_assert(ip4config_path[0] == '/');
            break;
        }
    }
    g_assert(active_device_found);

    network_manager_free_devices(devices);
    g_free(ip4config_path);
}

void test_device_addresses ()
{
    gchar ** devices, ** device_pointer;
    guint32 device_state;
    network_manager_ip4config ** addresses, ** address_pointer;
    network_manager_ip4config * address;
    guint address_count;

    devices = network_manager_get_devices();
    g_assert(devices);

    addresses = NULL;
    for (
            device_pointer = devices;
            *device_pointer != NULL;
            device_pointer++
        )
    {
        device_state = network_manager_get_device_state(*device_pointer);

        if (device_state == NM_DEVICE_STATE_ACTIVATED)
        {
            addresses = network_manager_get_addresses(*device_pointer);
            network_manager_free_devices(devices);
            break;
        }
    }
    g_assert(addresses);

    for (
            address_pointer = addresses, address_count = 0;
            *address_pointer != NULL;
            address_pointer++, address_count++
        )
    {
        address = *address_pointer;
        g_assert_cmpuint(address->ip_address, >, 0);
        g_assert_cmphex(address->ip_address, <, 0xFFFFFFFF);
        g_assert_cmpuint(address->prefix, >, 0);
        g_assert_cmpuint(address->prefix, <, 32);
        g_assert_cmpuint(address->gateway_address, >, 0);
        g_assert_cmphex(address->gateway_address, <, 0xFFFFFFFF);
    }
    network_manager_free_addresses(addresses);
    g_assert_cmpuint(address_count, >, 0);
}

void test_device_configurations ()
{
    network_manager_device_config ** configurations;
    guint configuration_count;

    configurations = network_manager_device_configurations();
    g_assert(configurations);

    for (
            configuration_count = 0;
            configurations[configuration_count] != NULL;
            configuration_count++
        )
    {
        g_assert(configurations[configuration_count]->device_name);
        g_assert_cmpuint(
                strlen(configurations[configuration_count]->device_name),
                >,
                0
            );

        g_assert(configurations[configuration_count]->ip_config);
        g_assert(configurations[configuration_count]->ip_config[0]);
        g_assert_cmpuint(
                configurations[configuration_count]->ip_config[0]->prefix,
                >,
                0
            );
        g_assert_cmpuint(
                configurations[configuration_count]->ip_config[0]->prefix,
                <,
                32
            );
    }
    g_assert_cmpuint(configuration_count, >, 0);
    network_manager_free_device_configurations(configurations);
}
