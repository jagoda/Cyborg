#include <check.h>
#include <stdlib.h>

#include "network-manager.h"


int main ()
{
    int number_failed;
    SRunner * runner;
   
    runner = srunner_create(network_manager_suite());
    number_failed = 0;

    srunner_run_all(runner, CK_NORMAL);
    number_failed = srunner_ntests_failed(runner);
    srunner_free(runner);

    return number_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
