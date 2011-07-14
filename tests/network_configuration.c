#include <arpa/inet.h>
#include <check.h>
#include <netinet/in.h>
#include <string.h>

#include "../src/network.h"
#include "network_configuration.h"


static const gchar * configuration_file = "test_config.json";


static ip4_config ** synthesize_configurations ();


START_TEST(test_parse)
{
    network_configuration ** configurations, ** configuration_pointer;
    network_configuration * configuration;
    gint configurations_tested;

    configurations = network_configuration_parse(configuration_file);
    configurations_tested = 0;
    for (
            configuration_pointer = configurations;
            *configuration_pointer != NULL;
            configuration_pointer++
        )
    {
        configuration = *configuration_pointer;
        fail_unless(
                strlen(configuration->server) > 0,
                "Invalid server name."
            );
        fail_unless(configuration->address > 0, "Invalid address.");
        fail_unless(configuration->prefix > 0, "Invalid prefix.");
        fail_unless(configuration->gateway > 0, "Invalid gateway.");
        configurations_tested++;
    }
    network_configuration_free(configurations);

    fail_unless(configurations_tested > 0, "No configurations tested.");
}
END_TEST

START_TEST(test_match)
{
    network_configuration ** network_configurations;
    ip4_config ** ip_status;
    gchar * server;

    network_configurations = network_configuration_parse(configuration_file);
    ip_status = synthesize_configurations();
    server = network_configuration_match(
            network_configurations,
            ip_status
        );
    network_configuration_free(network_configurations);
    network_manager_free_addresses(ip_status);

    fail_if(server == NULL, "no network match found");
    fail_unless(
            strcmp("bump.dyn.webahead.ibm.com", server) == 0,
            "network match returned incorrect server"
        );
    g_free(server);
}
END_TEST


TCase * network_configuration_core_testcase ()
{
    TCase * testcase;

    testcase = tcase_create("Core");
    tcase_add_test(testcase, test_parse);
    tcase_add_test(testcase, test_match);
    
    return testcase;
}

Suite * network_configuration_suite ()
{
    Suite * suite;
    
    suite = suite_create("Network Configuration");
    suite_add_tcase(suite, network_configuration_core_testcase());

    return suite;
}


ip4_config ** synthesize_configurations ()
{
    ip4_config ** configurations;

    g_debug("allocating memory for synthesized configurations");
    configurations = (ip4_config **) g_malloc(
            2 * sizeof(ip4_config *)
        );
    configurations[0] = (ip4_config *) g_malloc(
            sizeof(ip4_config)
        );
    configurations[1] = NULL;

    g_debug("creating fake configuration entry");
    configurations[0]->address = ntohl(inet_addr("9.37.31.154"));
    configurations[0]->prefix = 25;
    configurations[0]->gateway = ntohl(inet_addr("9.37.31.129"));

    return configurations;
}
