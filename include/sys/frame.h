#pragma once

#ifdef __x86_64__
#include <sys/frame_x86_64.h>
#else
#error "no supported architecture found"
#endif
