#ifndef TEST_MODULE_H
#define TEST_MODULE_H

#include <glib.h>


#define register_test(test) \
    g_test_add_func("/" __FILE__ "/" #test, test)


void register_tests ();

#endif /* TEST_MODULE_H */
