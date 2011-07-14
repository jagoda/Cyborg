#ifndef NETWORK_H
#define NETWORK_H

#include <gio/gio.h>

#include "network_configuration.h"
#include "network-manager.h"


ip4_config ** network_my_addresses (GDBusConnection * connection);

void network_free_addresses (ip4_config ** addresses);

#endif /* NETWORK_H */
