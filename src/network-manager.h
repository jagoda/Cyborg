#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <gio/gio.h>


GDBusConnection * network_manager_init ();
GVariant * network_manager_get_devices (GDBusConnection * connection);

#endif /* NETWORK_MANAGER_H */
