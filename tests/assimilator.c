#include <arpa/inet.h>
#include <glib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>

#include "test_module.h"
#include "../src/assimilator.h"
#include "../src/configuration_parser.h"
#include "../src/network-manager.h"


#define TEST_CONFIGURATION      "test.conf"


/* Test cases. */
static void test_match_network ();
static void test_connect ();
static void test_disconnect ();

static network_manager_device_config ** synthesize_configuration ();


void register_tests ()
{
    register_test(test_match_network);
    register_test(test_connect);
    register_test(test_disconnect);
}


void test_match_network ()
{
    network_manager_device_config ** device_configuration;
    server_configuration ** stored_configuration;
    const server_configuration * matched_configuration;

    device_configuration = synthesize_configuration();
    stored_configuration = configuration_parser_load(TEST_CONFIGURATION);
    matched_configuration = assimilator_match_network(
            stored_configuration,
            device_configuration
        );
    network_manager_free_device_configurations(device_configuration);

    g_assert(matched_configuration);
    g_assert(matched_configuration->server);
    g_assert_cmpstr(
            "fake.synergy.server.com",
            ==,
            matched_configuration->server
        );

    /* NOTE: matched configuration pointer points inside searched block. */
    configuration_parser_free_configurations(stored_configuration);
}

void test_connect ()
{
    g_assert(assimilator_connect(TEST_CONFIGURATION));
}

void test_disconnect ()
{
    g_assert(assimilator_disconnect());
}


network_manager_device_config ** synthesize_configuration ()
{
    network_manager_device_config ** device_config;
    struct in_addr ip_address;

    device_config = (network_manager_device_config **) g_malloc(
            2 * sizeof(network_manager_device_config *)
        );
    device_config[0] = (network_manager_device_config *) g_malloc(
            sizeof(network_manager_device_config)
        );
    device_config[1] = NULL;
    device_config[0]->ip_config =
        (network_manager_ip4config **) g_malloc(
                2 * sizeof(network_manager_ip4config *)
            );
    device_config[0]->ip_config[0] =
        (network_manager_ip4config *) g_malloc(
                sizeof(network_manager_ip4config)
            );
    device_config[0]->ip_config[1] = NULL;

    device_config[0]->device_name = g_strdup("eth0");
    inet_aton("9.37.31.200", &ip_address);
    device_config[0]->ip_config[0]->ip_address = ntohl(ip_address.s_addr);
    inet_aton("9.37.31.129", &ip_address);
    device_config[0]->ip_config[0]->gateway_address = ntohl(ip_address.s_addr);
    device_config[0]->ip_config[0]->prefix = 25;

    return device_config;
}
