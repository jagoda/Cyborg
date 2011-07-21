#include <fcntl.h>
#include <glib.h>
#include <glib-object.h>
#include <ltdl.h>
#include <stdlib.h>


#define MODULE_PATTERN      "*.la"
#define MODULE_INITIALIZER  "register_tests"


int main (int argc, char ** argv)
{
    gchar * current_directory, * module_name;
    gchar * current_path, * new_path;
    GDir * test_directory;
    GPatternSpec * test_pattern;
    GError * error;

    lt_dlhandle module_handle;
    void (*module_initializer)();

    g_debug("Initializing dependencies.");
    g_type_init();
    g_test_init(&argc, &argv, NULL);
    if (lt_dlinit())
    {
        g_error("Failed to initialize LTDL: %s.", lt_dlerror());
    }
    if (! (current_path = getenv("PATH")))
    {
        g_error("Failed to get PATH variable.");
    }
    new_path = g_strconcat("./:", current_path, NULL);
    if (setenv("PATH", new_path, TRUE))
    {
        g_error("Failed to update PATH variable.");
    }
    g_free(new_path);
    current_directory = g_get_current_dir();
    if (lt_dlsetsearchpath(current_directory))
    {
        g_error("Failed to update LTDL search path: %s.", lt_dlerror());
    }
    g_debug("LTDL search path is: %s.", lt_dlgetsearchpath());

    g_debug("Searching for test modules...");
    error = NULL;
    if (! (test_directory = g_dir_open(
                current_directory,
                O_RDONLY,
                &error
            ))
        )
    {
        g_error("Failed to open test directory: %s.", error->message);
        g_error_free(error);
        error = NULL;
    }
    g_free(current_directory);
    test_pattern = g_pattern_spec_new(MODULE_PATTERN);
    while ((module_name = (gchar *) g_dir_read_name(test_directory)))
    {
        if (g_pattern_match_string(test_pattern, module_name))
        {
            g_debug("Found test module: %s", module_name);
            if ((module_handle = lt_dlopen(module_name)))
            {
                if ((
                        module_initializer =
                        lt_dlsym(module_handle, MODULE_INITIALIZER)
                   ))
                {
                    g_debug("Registering tests for %s", module_name);
                    module_initializer();
                }
                else
                {
                    g_error(
                            "Failed to register tests for %s: %s",
                            module_name,
                            lt_dlerror()
                        );
                }
            }
            else
            {
                g_error(
                        "Failed to load module %s: %s",
                        module_name,
                        lt_dlerror()
                    );
            }
        }
    }
    g_pattern_spec_free(test_pattern);
    

    return g_test_run();
}
