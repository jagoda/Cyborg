#include <check.h>

#include "../src/network.h"
#include "../src/network-manager.h"


START_TEST(test_my_addresses)
{
    ip4_config ** addresses, ** address_pointer;
    gint address_count;

    addresses = network_my_addresses();
    fail_if(addresses == NULL, "network_my_addresses returned NULL.");
    for (
            address_count = 0, address_pointer = addresses;
            *address_pointer != NULL;
            address_pointer++, address_count++
        );
    fail_unless(address_count > 0, "Really? No local addresses?");
}
END_TEST


TCase * network_core_testcase ()
{
    TCase * testcase;

    testcase = tcase_create("Core");
    tcase_add_test(testcase, test_my_addresses);

    return testcase;
}

Suite * network_suite ()
{
    Suite * suite;

    suite = suite_create("Network");
    suite_add_tcase(suite, network_core_testcase());

    return suite;
}
