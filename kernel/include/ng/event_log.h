#pragma once

#include "sys/cdefs.h"
#include <sys/event_log.h>

BEGIN_DECLS

void event_log_init();
void log_event(enum event_type type, const char *message, ...);

END_DECLS
