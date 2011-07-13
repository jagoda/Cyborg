#include <check.h>
#include <string.h>

#include "../src/network.h"
#include "network_configuration.h"


static const gchar * configuration_file = "test_config.json";


static void setup ();
static void teardown ();


void setup ()
{
    network_init();
}

void teardown ()
{
    /* TODO: implement */
}

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
        /* FIXME: doesn't catch uninitialized values */
        fail_unless(configuration->address > 0, "Invalid address.");
        fail_unless(configuration->prefix > 0, "Invalid prefix.");
        fail_unless(configuration->gateway > 0, "Invalid gateway.");
        configurations_tested++;
    }
    network_configuration_free(configurations);

    fail_unless(configurations_tested > 0, "No configurations tested.");
}
END_TEST


TCase * network_configuration_core_testcase ()
{
    TCase * testcase;

    testcase = tcase_create("Core");
    tcase_add_checked_fixture(testcase, setup, teardown);
    tcase_add_test(testcase, test_parse);
    
    return testcase;
}

Suite * network_configuration_suite ()
{
    Suite * suite;
    
    suite = suite_create("Network Configuration");
    suite_add_tcase(suite, network_configuration_core_testcase());

    return suite;
}
