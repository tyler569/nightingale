
#pragma once
#ifndef NG_SERIAL_H
#define NG_SERIAL_H

#include <basic.h>

/*
 * If the system supports multiple serual ports, those can be accessed
 * through architecture-specific code. This ecists to give a common
 * interface to the default serial terminal only.
 */

void serial_init(void);

void serial_write(const char c);
void serial_write_str(const char *buf, size_t len);
char serial_read(void);

void serial2_write(const char c);
void serial2_write_str(const char *buf, size_t len);
char serial2_read(void);

#endif // NG_SERIAL_H

