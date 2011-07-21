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

static network_manager_ip4config ** synthesize_configuration ();


void register_tests ()
{
    register_test(test_match_network);
    register_test(test_connect);
}


void test_match_network ()
{
    network_manager_ip4config ** ip_configuration;
    server_configuration ** stored_configuration;
    const server_configuration * matched_configuration;

    ip_configuration = synthesize_configuration();
    stored_configuration = configuration_parser_load(TEST_CONFIGURATION);
    matched_configuration = assimilator_match_network(
            stored_configuration,
            ip_configuration
        );
    network_manager_free_addresses(ip_configuration);

    g_assert(matched_configuration);
    g_assert(matched_configuration->server);
    g_assert_cmpstr(
            "bump.dyn.webahead.ibm.com",
            ==,
            matched_configuration->server
        );

    /* NOTE: matched configuration pointer points inside searched block. */
    configuration_parser_free_configurations(stored_configuration);
}

void test_connect ()
{
    /* TODO: implement */
    g_assert(FALSE);
}


network_manager_ip4config ** synthesize_configuration ()
{
    network_manager_ip4config ** ip_configuration;
    struct in_addr ip_address;

    ip_configuration = (network_manager_ip4config **) g_malloc(
            2 * sizeof(network_manager_ip4config *)
        );
    ip_configuration[0] = (network_manager_ip4config *) g_malloc(
            sizeof(network_manager_ip4config)
        );
    ip_configuration[1] = NULL;

    inet_aton("9.37.31.200", &ip_address);
    ip_configuration[0]->ip_address = ntohl(ip_address.s_addr);
    inet_aton("9.37.31.129", &ip_address);
    ip_configuration[0]->gateway_address = ntohl(ip_address.s_addr);
    ip_configuration[0]->prefix = 25;

    return ip_configuration;
}
