#pragma once

#ifndef _X86_RTC_H_
#define _X86_RTC_H_

#include <sys/cdefs.h>

BEGIN_DECLS

struct tm rtc_now(void);

END_DECLS

#endif
