#ifndef SYNERGY_H
#define SYNERGY_H

#include <glib.h>

#include "types.h"


gboolean synergy_connect (const server_configuration * configuration);

gboolean synergy_disconnect ();

#endif  /* SYNERGY_H */
