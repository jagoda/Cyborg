#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#define CALLBACK(type)     void (*)(type)


void network_manager_get_devices (CALLBACK(gchar **));

#endif /* NETWORK_MANAGER_H */
