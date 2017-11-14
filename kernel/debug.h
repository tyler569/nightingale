
#pragma once

#include <term/print.h>

#ifdef DEBUG
#define DEBUG_PRINTF(args...) printf("[DEBUG] " args);
#else
#define DEBUG_PRINTF(args...) do {} while(0);
#endif

