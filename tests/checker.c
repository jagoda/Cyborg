#include <glib.h>
#include <fcntl.h>
#include <ltdl.h>
#include <stdlib.h>


#define MODULE_PATTERN      "*.la"
#define MODULE_INITIALIZER  "register_tests"


int main (int argc, char ** argv)
{
    GError * error;
    GPatternSpec * pattern;
    GDir * test_directory;
    gchar * module_name, * current_directory;

    void * module_handle;
    void (*initializer)();


    g_debug("Initializing libraries.");
    g_test_init(&argc, &argv, NULL);
    if (lt_dlinit())
    {
        g_error(
                "Failed to initialized the Libtool loader: %s.",
                lt_dlerror()
            );
    }
    current_directory = g_get_current_dir();
    if (lt_dlsetsearchpath(current_directory))
    {
        g_error("Failed to update search path: %s.", lt_dlerror());
    }
    g_debug("LTDL search path is: %s", lt_dlgetsearchpath());

    g_debug("Searching for tests...");
    error = NULL;
    test_directory = g_dir_open(current_directory, O_RDONLY, &error);
    if (error)
    {
        g_error("Failed to open test directory: %s.", error->message);
        g_error_free(error);
        error = NULL;
    }
    g_free(current_directory);

    pattern = g_pattern_spec_new(MODULE_PATTERN);
    while (module_name = (gchar *) g_dir_read_name(test_directory))
    {
        if (g_pattern_match_string(pattern, module_name))
        {
            g_debug("Found test module '%s'.", module_name);

            if (module_handle = lt_dlopen(module_name))
            {
                if (
                        initializer = 
                        lt_dlsym(module_handle, MODULE_INITIALIZER)
                   )
                {
                    g_debug("Registering tests from '%s'.", module_name);
                    initializer();
                }
                else
                {
                    g_error(
                            "Failed to register tests from '%s': %s.",
                            module_name,
                            lt_dlerror()
                        );
                }
            }
            else
            {
                g_error(
                        "Failed to load module '%s': %s.",
                        module_name,
                        lt_dlerror()
                    );
            }
        }
    }
    g_pattern_spec_free(pattern);

    return g_test_run();
}
