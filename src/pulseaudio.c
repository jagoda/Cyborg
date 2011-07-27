#include <glib.h>
#include <pulse/pulseaudio.h>

#include "pulseaudio.h"


#define WAIT_FOR_COMPLETION(condition, mainloop)        \
    while ((condition))                                 \
    {                                                   \
        if (pa_mainloop_iterate(mainloop, 0, NULL) < 0) \
        {                                               \
            g_error("Mainloop iteration failed.");      \
            break;                                      \
        }                                               \
    }

#define MODULE_LOOPBACK             "module-loopback"
#define MODULE_TUNNEL_SOURCE        "module-tunnel-source"


static void pulseaudio_init (pa_mainloop ** mainloop, pa_context ** context);

static void pulseaudio_destroy (pa_mainloop * mainloop, pa_context * context);

static void load_module_callback (
        pa_context * context,
        uint32_t index,
        void * userdata
    );


gboolean pulseaudio_connect (
        gchar * server,
        guint source_index,
        guint sink_index
    )
{
    pa_mainloop * mainloop = NULL;
    pa_context * context = NULL;
    pa_operation * operation = NULL;
    gint32 module_index = 0;
    gchar * module_arguments = NULL;

    pulseaudio_init(&mainloop, &context);

    module_arguments = g_strdup_printf(
            "server=%s source=%u",
            server,
            source_index
        );
    operation = pa_context_load_module(
            context,
            MODULE_TUNNEL_SOURCE,
            module_arguments,
            load_module_callback,
            &module_index
        );
    g_free(module_arguments);
    WAIT_FOR_COMPLETION(
            pa_operation_get_state(operation) != PA_OPERATION_DONE,
            mainloop
        );
    pa_operation_unref(operation);
    /* FIXME: need to get source from server. */
    module_arguments = g_strdup_printf(
            "source=%u sink=%u",
            0,
            sink_index
        );
    /* FIXME: need to collect load statuses. */
    operation = pa_context_load_module(
            context,
            MODULE_LOOPBACK,
            module_arguments,
            load_module_callback,
            &module_index
        );
    g_free(module_arguments);
    WAIT_FOR_COMPLETION(
            pa_operation_get_state(operation) != PA_OPERATION_DONE,
            mainloop
        );
    pa_operation_unref(operation);

    pulseaudio_destroy(mainloop, context);

    /* FIXME: need to store the module indexes for disconnect. */

    return module_index > 0;
}

gboolean pulseaudio_disconnect ()
{
    pa_mainloop * mainloop = NULL;
    pa_context * context = NULL;

    pulseaudio_init(&mainloop, &context);
    pulseaudio_destroy(mainloop, context);

    return FALSE;
}


void pulseaudio_init (pa_mainloop ** mainloop, pa_context ** context)
{
    if (mainloop == NULL || context == NULL)
    {
        g_error("NULL pointer reference.");
    }
    else if (*mainloop == NULL && *context == NULL)
    {
        *mainloop = pa_mainloop_new();
        *context = pa_context_new(
                pa_mainloop_get_api(*mainloop),
                "Cyborg PulseAudio Controller"
            );
        if (pa_context_connect(*context, NULL, PA_CONTEXT_NOAUTOSPAWN, NULL))
        {
            g_error("Failed to connect to PulseAudio server.");
        }
        /* Wait for context to connect. */
        WAIT_FOR_COMPLETION(
                pa_context_get_state(*context) != PA_CONTEXT_READY,
                *mainloop
            );
    }
    else
    {
        g_error("Arguments seem to already be initialized.");
    }
}

void pulseaudio_destroy (pa_mainloop * mainloop, pa_context * context)
{
    if (mainloop != NULL && context != NULL)
    {
        pa_context_unref(context);
        pa_mainloop_free(mainloop);
    }
}

void load_module_callback (
        pa_context * context,
        uint32_t index,
        void * userdata
    )
{
    int * return_value_pointer = (int *) userdata;

    if (return_value_pointer)
    {
        *return_value_pointer = index;
    }
}
