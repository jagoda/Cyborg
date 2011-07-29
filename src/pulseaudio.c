#include <glib.h>
#include <pulse/pulseaudio.h>

#include "assimilator.h"
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

static void success_callback (
        pa_context * context,
        int success,
        void * userdata
    );

static void lookup_source_callback (
        pa_context * context,
        const pa_source_info * sources,
        int list_length,
        void * userdata
    );

static gint create_source_tunnel (
        pa_context * context,
        pa_mainloop * mainloop,
        gchar * server,
        guint source_index
    );

static gint create_loopback (
        pa_context * context,
        pa_mainloop * mainloop,
        guint source_index,
        guint sink_index
    );

static gboolean unload_module (
        pa_context * context,
        pa_mainloop * mainloop,
        guint module_index
    );

static guint lookup_loaded_source (
        pa_context * context,
        pa_mainloop * mainloop,
        guint module_index
    );


gboolean pulseaudio_connect (
        server_configuration * configuration,
        gint * tunnel_module_index,
        gint * loopback_module_index
    )
{
    pa_mainloop * mainloop = NULL;
    pa_context * context = NULL;
    guint tunnel_source = 0;

    if (! configuration)
    {
        g_error("Configuration pointer must not be NULL.");
    }

    pulseaudio_init(&mainloop, &context);
    if (
            (*tunnel_module_index =
                create_source_tunnel(
                    context,
                    mainloop,
                    configuration->server,
                    configuration->audio_configuration->source
                )
            )
            < 0
        )
    {
        g_error("Failed to connect to server.");
    }
    if (
            (tunnel_source = lookup_loaded_source(
                context,
                mainloop,
                *tunnel_module_index)
            )
            ==
            PA_INVALID_INDEX
        )
    {
        g_error("Failed to get index for new source.");
    }
    if (
            (*loopback_module_index =
                 create_loopback(
                     context,
                     mainloop,
                     tunnel_source,
                     configuration->audio_configuration->sink
                )
            )
            < 0
        )
    {
        g_error("Failed to link local and remote sources.");
    }
    pulseaudio_destroy(mainloop, context);

    return *tunnel_module_index > 0 && *loopback_module_index > 0;
}

gboolean pulseaudio_disconnect (
        gint tunnel_module_index,
        gint loopback_module_index
    )
{
    pa_mainloop * mainloop = NULL;
    pa_context * context = NULL;
    gboolean tunnel_unloaded = FALSE, loopback_unloaded = FALSE;

    pulseaudio_init(&mainloop, &context);
    tunnel_unloaded = unload_module(context, mainloop, tunnel_module_index);
    loopback_unloaded = unload_module(
            context,
            mainloop,
            loopback_module_index
        );
    pulseaudio_destroy(mainloop, context);

    return tunnel_unloaded && loopback_unloaded;
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

void success_callback (
        pa_context * context,
        int success,
        void * userdata
    )
{
    int * return_value_pointer = (int *) userdata;

    if (return_value_pointer)
    {
        *return_value_pointer = success;
    }
}

void lookup_source_callback (
        pa_context * context,
        const pa_source_info * sources,
        int end_of_list,
        void * userdata
    )
{
    gint index = 0;
    guint * module_index = userdata;

    /* FIXME: need to be able to set to invalid if not found. */
    if (end_of_list == 0)
    {
        if (sources[index].owner_module == *module_index)
        {
            *module_index = sources[index].index;
        }
    }
    else if (end_of_list < 0)
    {
        *module_index = PA_INVALID_INDEX;
    }
}

gint create_source_tunnel (
        pa_context * context,
        pa_mainloop * mainloop,
        gchar * server,
        guint source_index
    )
{
    gchar * module_arguments = NULL;
    pa_operation * operation = NULL;
    gint32 module_index = -1;

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

    return module_index;
}

gint create_loopback (
        pa_context * context,
        pa_mainloop * mainloop,
        guint source_index,
        guint sink_index
    )
{
    gchar * module_arguments = NULL;
    pa_operation * operation = NULL;
    gint32 module_index = -1;

    module_arguments = g_strdup_printf(
            "source=%u sink=%u",
            source_index,
            sink_index
        );
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

    return module_index;
}

gboolean unload_module (
        pa_context * context,
        pa_mainloop * mainloop,
        guint module_index)
{
    pa_operation * operation = NULL;
    gint success = 0;

    operation = pa_context_unload_module(
            context,
            module_index,
            success_callback,
            &success
        );
    WAIT_FOR_COMPLETION(
            pa_operation_get_state(operation) != PA_OPERATION_DONE,
            mainloop
        );

    return success != 0;
}

guint lookup_loaded_source (
        pa_context * context,
        pa_mainloop * mainloop,
        guint module_index
    )
{
    pa_operation * operation = NULL;

    /*
        NOTE: the callback function uses a single pointer as input and output.
        The starting value should be the owning module index. The final
        value will be the source index owned by that module.
    */
    operation = pa_context_get_source_info_list(
            context,
            lookup_source_callback,
            &module_index
        );
    WAIT_FOR_COMPLETION(
            pa_operation_get_state(operation) != PA_OPERATION_DONE,
            mainloop
        );

    return module_index;
}
