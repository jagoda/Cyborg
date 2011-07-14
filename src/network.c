#include <glib.h>

#include "network.h"


ip4_config ** network_my_addresses (GDBusConnection * connection)
{
    GPtrArray * addresses;
    ip4_config ** device_addresses, ** address_pointer;
    gchar ** devices, ** device_pointer, * device_path;
    gint index;

    g_debug("asking DBus for list of devices");
    addresses = g_ptr_array_new();
    devices = network_manager_get_devices(connection);
    for (
            device_pointer = devices;
            *device_pointer != NULL;
            device_pointer++
        )
    {
        device_path = *device_pointer;
        if (network_manager_device_state(connection, device_path) ==
                NM_DEVICE_STATE_ACTIVATED)
        {
            g_debug("found active device '%s'", device_path);
            device_addresses = network_manager_device_addresses(
                    connection,
                    device_path
                );
            g_debug("adding device addresses to master list");
            for (
                    address_pointer = device_addresses;
                    *address_pointer != NULL;
                    address_pointer++
                )
            {
                g_ptr_array_add(
                        addresses,
                        g_memdup(*address_pointer, sizeof(ip4_config))
                    );
            }
            /* FIXME: this is going to erase the contents pointed too... */
            network_manager_free_addresses(device_addresses);
        }
    }
    network_manager_free_devices(devices);

    g_debug("trimming list");
    device_addresses = (ip4_config **) g_malloc(
            (addresses->len + 1) * sizeof(ip4_config *)
        );
    for (index = 0; index < addresses->len; index++)
    {
        device_addresses[index] = g_ptr_array_index(addresses, index);
    }
    device_addresses[index] = NULL;
    g_ptr_array_free(addresses, TRUE);

    return device_addresses;
}

void network_free_addresses (ip4_config ** addresses)
{
    network_manager_free_addresses(addresses);
}
