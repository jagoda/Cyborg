#ifndef NETWORK_H
#define NETWORK_H

#include "network_configuration.h"
#include "network-manager.h"


void network_init ();

ip4_config ** network_my_addresses ();

void network_free_addresses (ip4_config ** addresses);

#endif /* NETWORK_H */
