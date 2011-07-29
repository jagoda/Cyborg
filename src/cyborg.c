#include <glib.h>
#include <glib-object.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "assimilator.h"


#define CONFIG_FILE     ".cyborg.conf"
#define PID_FILE        ".cyborg.pid"


static gchar * configuration_path ();

static gboolean check_pid_file (gchar * pid_file);

static gboolean write_pid_file (gchar * pid_file, guint pid);

static void connect_handler (guint state);

static void exit_handler (int signum);


int main (int argc, char ** argv)
{
    gchar * pid_file;
    guint child_pid;
    GMainLoop * event_loop;

    g_type_init();

    pid_file = g_strconcat(getenv("HOME"), "/", PID_FILE, NULL);
    if (! check_pid_file(pid_file))
    {
        if (( child_pid = fork() ))
        {
            if (! write_pid_file(pid_file, child_pid))
            {
                g_warning("Killing child process since PID file creation failed.");
                kill(child_pid, SIGTERM);
            }
        }
        else
        {
            signal(SIGTERM, exit_handler);
            connect_handler(NM_DEVICE_STATE_ACTIVATED);
            event_loop = g_main_loop_new(NULL, FALSE);
            network_manager_register_connect_handler(connect_handler);
            g_main_loop_run(event_loop);
        }
    }
    g_free(pid_file);

    return EXIT_SUCCESS;
}


gchar * configuration_path ()
{
    return g_strconcat(getenv("HOME"), "/", CONFIG_FILE, NULL);
}

gboolean check_pid_file (gchar * pid_file)
{
    gchar * file_contents;
    guint current_pid;
    gboolean service_is_running;

    if (! pid_file)
    {
        g_error("File name for PID file cannot be NULL.");
    }

    service_is_running = FALSE;
    if (g_file_get_contents(pid_file, &file_contents, NULL, NULL))
    {
        current_pid = atoi(file_contents);
        if (kill(current_pid, 0) == 0)
        {
            service_is_running = TRUE;
        }
    }

    return service_is_running;
}

gboolean write_pid_file (gchar * pid_file, guint pid)
{
    gchar buffer[G_ASCII_DTOSTR_BUF_SIZE];
    gboolean success;
    GError * error;

    error = NULL;
    success = TRUE;
    g_ascii_dtostr(buffer, G_ASCII_DTOSTR_BUF_SIZE, pid);
    if (g_file_set_contents(pid_file, buffer, -1, &error) == FALSE)
    {
        success = FALSE;
        g_error("Failed to create PID file: %s", error->message);
        g_error_free(error);
        error = NULL;
    }

    return success;
}

void connect_handler (guint state)
{
    gchar * configuration_file;

    switch(state)
    {
        case NM_DEVICE_STATE_UNAVAILABLE:
        case NM_DEVICE_STATE_DISCONNECTED:
        case NM_DEVICE_STATE_ACTIVATED:
            configuration_file = configuration_path();
            assimilator_disconnect();
            assimilator_connect(configuration_file);
            g_free(configuration_file);
            break;
        default:
            break;
    }
}

void exit_handler (int signum)
{
    gchar * pid_file = NULL;

    assimilator_disconnect();
    pid_file = g_strconcat(getenv("HOME"), "/", PID_FILE, NULL);
    unlink(pid_file);
    g_free(pid_file);

    exit(EXIT_SUCCESS);
}
