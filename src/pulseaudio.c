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
#define MODULE_TUNNEL_SINK          "module-tunnel-sink"


static void pulseaudio_init (pa_mainloop ** mainloop, pa_context ** context);

static void pulseaudio_destroy (pa_mainloop * mainloop, pa_context * context);

gboolean connect_pull (
        pa_context * context,
        pa_mainloop * mainloop,
        server_configuration * configuration,
        GQueue * loaded_modules
    );

gboolean connect_push (
        pa_context * context,
        pa_mainloop * mainloop,
        server_configuration * configuraiton,
        GQueue * loaded_modules
    );

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
        int end_of_list,
        void * userdata
    );

static void lookup_sink_callback (
        pa_context * context,
        const pa_sink_info * sinks,
        int end_of_list,
        void * userdata
    );

static gint create_source_tunnel (
        pa_context * context,
        pa_mainloop * mainloop,
        gchar * server,
        guint source_index
    );

static gint create_sink_tunnel (
        pa_context * context,
        pa_mainloop * mainloop,
        gchar * server,
        guint sink_index
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

static gchar * lookup_loaded_sink (
        pa_context * context,
        pa_mainloop * mainloop,
        guint module_index
    );

static gboolean set_default_sink (
        pa_context * context,
        pa_mainloop * mainloop,
        gchar * sink_name
    );


gboolean pulseaudio_connect (
        server_configuration * configuration,
        GQueue * loaded_modules
    )
{
    pa_mainloop * mainloop = NULL;
    pa_context * context = NULL;
    gboolean success = FALSE;

    if (! configuration)
    {
        g_error("Configuration pointer must not be NULL.");
    }

    pulseaudio_init(&mainloop, &context);
    switch(configuration->audio_configuration->mode)
    {
        case PULSEAUDIO_MODE_PULL:
            success = connect_pull(
                    context,
                    mainloop,
                    configuration,
                    loaded_modules
                );
            break;
        case PULSEAUDIO_MODE_PUSH:
            success = connect_push(
                    context,
                    mainloop,
                    configuration,
                    loaded_modules
                );
            break;
        default:
            g_error("Invalid audio mode.");
            break;
    }
    pulseaudio_destroy(mainloop, context);

    return success;
}

gboolean pulseaudio_disconnect (GQueue * loaded_modules)
{
    pa_mainloop * mainloop = NULL;
    pa_context * context = NULL;
    gboolean success = TRUE;

    pulseaudio_init(&mainloop, &context);
    while(! g_queue_is_empty(loaded_modules))
    {
        success &= unload_module(
                context,
                mainloop,
                GPOINTER_TO_UINT(g_queue_pop_head(loaded_modules))
            );
    }
    pulseaudio_destroy(mainloop, context);

    return success;
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

gboolean connect_pull (
        pa_context * context,
        pa_mainloop * mainloop,
        server_configuration * configuration,
        GQueue * loaded_modules
    )
{
    guint tunnel_source_index = PA_INVALID_INDEX;
    guint tunnel_module_index = PA_INVALID_INDEX;
    guint loopback_module_index = PA_INVALID_INDEX;
    gboolean success = TRUE;

    if (
            (tunnel_module_index =
                create_source_tunnel(
                    context,
                    mainloop,
                    configuration->server,
                    configuration->audio_configuration->source
                )
            )
            ==
            PA_INVALID_INDEX
        )
    {
        g_error("Failed to connect to server.");
        success = FALSE;
    }
    else
    {
        g_queue_push_head(
                loaded_modules,
                GUINT_TO_POINTER(tunnel_module_index)
            );
    }
    if (
            (tunnel_source_index = lookup_loaded_source(
                context,
                mainloop,
                tunnel_module_index)
            )
            ==
            PA_INVALID_INDEX
        )
    {
        g_error("Failed to get index for new source.");
        success = FALSE;
    }
    if (
            (loopback_module_index =
                 create_loopback(
                     context,
                     mainloop,
                     tunnel_source_index,
                     configuration->audio_configuration->sink
                )
            )
            ==
            PA_INVALID_INDEX
        )
    {
        g_error("Failed to link local and remote sources.");
        success = FALSE;
    }
    else
    {
        g_queue_push_head(
                loaded_modules,
                GUINT_TO_POINTER(loopback_module_index)
            );
    }

    return success;
}

gboolean connect_push (
        pa_context * context,
        pa_mainloop * mainloop,
        server_configuration * configuration,
        GQueue * loaded_modules
    )
{
    guint tunnel_module_index = PA_INVALID_INDEX;
    gchar * loaded_sink_name = NULL;
    gboolean success = TRUE;

    if (
            (tunnel_module_index = create_sink_tunnel(
                context,
                mainloop,
                configuration->server,
                configuration->audio_configuration->sink
            ))
            ==
            PA_INVALID_INDEX
        )
    {
        g_error("Failed to create tunnel for sink.");
        success = FALSE;
    }
    else
    {
        g_queue_push_head(
                loaded_modules,
                GUINT_TO_POINTER(tunnel_module_index)
            );
    }
    if (
            (loaded_sink_name = lookup_loaded_sink(
                context,
                mainloop,
                tunnel_module_index
            ))
            ==
            NULL
       )
    {
        g_error("Failed to index for new sink.");
        success = FALSE;
    }
    if (!  set_default_sink(context, mainloop, loaded_sink_name))
    {
        g_error("Failed to update default sink.");
        success = FALSE;
    }
    g_free(loaded_sink_name);

    return success;
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
        const pa_source_info * source,
        int end_of_list,
        void * userdata
    )
{
    guint * module_index = userdata;

    /* FIXME: need to be able to set to invalid if not found. */
    if (end_of_list == 0)
    {
        if (source->owner_module == *module_index)
        {
            *module_index = source->index;
        }
    }
    else if (end_of_list < 0)
    {
        *module_index = PA_INVALID_INDEX;
    }
}

void lookup_sink_callback (
        pa_context * context,
        const pa_sink_info * sink,
        int end_of_list,
        void * userdata
    )
{
    guint module_index = *((guint *) userdata);

    /* FIXME: need to be able to set to NULL if not found. */
    if (end_of_list == 0)
    {
        if (sink->owner_module == module_index)
        {
            *((gchar **) userdata) = g_strdup(sink->name);
        }
    }
    else if (end_of_list < 0)
    {
        *((gchar **) userdata) = NULL;
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

gint create_sink_tunnel (
        pa_context * context,
        pa_mainloop * mainloop,
        gchar * server,
        guint sink_index
    )
{
    gchar * module_arguments = NULL;
    pa_operation * operation = NULL;
    gint32 module_index = -1;

    module_arguments = g_strdup_printf(
            "server=%s sink=%u",
            server,
            sink_index
        );
    operation = pa_context_load_module(
            context,
            MODULE_TUNNEL_SINK,
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

gchar * lookup_loaded_sink (
        pa_context * context,
        pa_mainloop * mainloop,
        guint module_index
    )
{
    pa_operation * operation = NULL;
    gpointer callback_data = GUINT_TO_POINTER(module_index);

    /*
        NOTE: the callback function uses a single pointer as input and output.
        The starting value should be the owning module index. The final
        value will be the source index owned by that module.
    */
    operation = pa_context_get_sink_info_list(
            context,
            lookup_sink_callback,
            &callback_data
        );
    WAIT_FOR_COMPLETION(
            pa_operation_get_state(operation) != PA_OPERATION_DONE,
            mainloop
        );

    return (gchar *) callback_data;
}

gboolean set_default_sink (
        pa_context * context,
        pa_mainloop * mainloop,
        gchar * sink_name
    )
{
    pa_operation * operation = NULL;
    gint success = 0;

    operation = pa_context_set_default_sink(
            context,
            sink_name,
            success_callback,
            &success
        );
    WAIT_FOR_COMPLETION(
            pa_operation_get_state(operation) != PA_OPERATION_DONE,
            mainloop
        );

    return success != 0;
}
