#include <ng/panic.h>
#include <ng/print.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

size_t fprintf(struct writer *writer, const char *format, ...) {
	va_list args;
	va_start(args, format);
	size_t ret = vfprintf(writer, format, args);
	va_end(args);
	return ret;
}

size_t fnprintf(struct writer *writer, size_t n, const char *format, ...) {
	va_list args;
	va_start(args, format);
	size_t ret = vfnprintf(writer, n, format, args);
	va_end(args);
	return ret;
}

size_t vfprintf(struct writer *writer, const char *format, va_list args) {
	return vfnprintf(writer, SIZE_MAX, format, args);
}

#define WRITE(data, size) \
	do { \
		writer->write(writer, data, size); \
		written += size; \
	} while (0)

enum padding {
	PADDING_NONE,
	PADDING_LEFT,
	PADDING_RIGHT,
};

enum base {
	BASE_10,
	BASE_16,
	BASE_16_CAPS,
	BASE_8,
	BASE_2,
	BASE_PTR,
};

struct format_spec {
	enum padding padding;
	enum base base;
	bool alternate_form;
	bool print_plus;
	char padding_char;
	short int_width;
	int padding_width;
};

struct number {
	uintmax_t value;
	bool negative;
};

static struct number get_number(
	struct format_spec *spec, bool signed_, va_list args) {
	uintmax_t absolute_value;
	bool negative = false;

	if (signed_ || spec->int_width < 0) {
		intmax_t value = 0;
		switch (spec->int_width) {
		case -2 ... 0:
			value = va_arg(args, int);
			break;
		case 1:
			value = va_arg(args, long);
			break;
		case 2:
			value = va_arg(args, long long);
			break;
		default:
			panic("invalid print width");
		}
		if (value < 0) {
			absolute_value = -value;
			negative = true;
		} else {
			absolute_value = value;
			negative = false;
		}
	} else {
		switch (spec->int_width) {
		case -2 ... 0:
			absolute_value = va_arg(args, unsigned);
			break;
		case 1:
			absolute_value = va_arg(args, unsigned long);
			break;
		case 2:
			absolute_value = va_arg(args, unsigned long long);
			break;
		default:
			panic("invalid print width");
		}
	}

	return (struct number) { absolute_value, negative };
}

const char *lower_hex_charset = "0123456789abcdef";
const char *upper_hex_charset = "0123456789ABCDEF";
#define NUM_MAX_DIGITS 64
#define NUM_BUF_SIZE (NUM_MAX_DIGITS + 1 + 2)

static size_t format_int(
	struct writer *writer, struct format_spec *spec, struct number number) {
	const char *charset = lower_hex_charset;
	char buf[NUM_BUF_SIZE + 1] = {};
	uintmax_t divisor;
	uintmax_t acc = number.value;

	switch (spec->base) {
	case BASE_10:
		divisor = 10;
		break;
	case BASE_16_CAPS:
		charset = upper_hex_charset;
		// fallthrough
	case BASE_16:
	case BASE_PTR:
		divisor = 16;
		break;
	case BASE_8:
		divisor = 8;
		break;
	case BASE_2:
		divisor = 2;
		break;
	default:
		panic("invalid print base");
	}

	int digit;
	for (digit = 0; digit < NUM_MAX_DIGITS; digit++) {
		buf[NUM_BUF_SIZE - digit] = charset[acc % divisor];
		acc /= divisor;
	}
	if (spec->alternate_form) {
		switch (spec->base) {
		case BASE_2:
			buf[NUM_BUF_SIZE - digit++] = 'b';
			buf[NUM_BUF_SIZE - digit++] = '0';
			break;
		case BASE_8:
			buf[NUM_BUF_SIZE - digit++] = '0';
			break;
		case BASE_10:
			break;
		case BASE_16_CAPS:
			buf[NUM_BUF_SIZE - digit++] = 'X';
			buf[NUM_BUF_SIZE - digit++] = '0';
			break;
		case BASE_16:
		case BASE_PTR:
			buf[NUM_BUF_SIZE - digit++] = 'x';
			buf[NUM_BUF_SIZE - digit++] = '0';
			break;
		}
	}
	if (number.negative)
		buf[NUM_BUF_SIZE - digit++] = '-';
	else if (spec->print_plus)
		buf[NUM_BUF_SIZE - digit++] = '+';

	return 0;
}

static size_t format_string(
	struct writer *writer, struct format_spec *spec, const char *s) {
	return 0;
}

size_t vfnprintf(
	struct writer *writer, size_t n, const char *format, va_list args) {
	size_t written = 0;

	while (*format != 0) {
		struct format_spec spec = {
			.base = BASE_10,
		};
		struct number number = {};

		size_t can_write = n - written;
		const char *p = strchr(format, '%');
		if (!p) {
			size_t remaining = strlen(format);
			if (remaining > can_write) {
				remaining = can_write;
			}
			WRITE(format, remaining);
			break;
		}

	consume_next:
		switch (*++format) {
		case '\0':
		case '%':
			WRITE("%", 1);
			break;
		case '-':
			spec.padding = PADDING_LEFT;
			spec.padding_char = ' ';
			goto consume_next;
		case '+':
			spec.print_plus = true;
			goto consume_next;
		case '0':
			spec.padding = PADDING_RIGHT;
			spec.padding_char = '0';
			goto consume_next;
		case '#':
			spec.alternate_form = true;
			goto consume_next;
		case '1' ... '9':
			if (spec.padding == PADDING_NONE)
				spec.padding = PADDING_RIGHT;
			spec.padding_width *= 10;
			spec.padding_width += *format - '0';
			goto consume_next;
		case 'l':
			spec.int_width++;
			goto consume_next;
		case 'h':
			spec.int_width--;
			goto consume_next;
		case 'j':
		case 'z':
		case 't':
		case 'L':
			spec.int_width = 2;
			goto consume_next;
		case 'c': {
			char c = va_arg(args, int);
			WRITE(&c, 1);
			break;
		}
		case 's': {
			const char *s = va_arg(args, const char *);
			format_string(writer, &spec, s);
			break;
		}
		case 'd':
		case 'i': {
			number = get_number(&spec, true, args);
			format_int(writer, &spec, number);
			break;
		}
		case 'u': {
			number = get_number(&spec, false, args);
			format_int(writer, &spec, number);
			break;
		}
		case 'x': {
			spec.base = BASE_16;
			number = get_number(&spec, false, args);
			format_int(writer, &spec, number);
			break;
		}
		case 'X': {
			spec.base = BASE_16_CAPS;
			number = get_number(&spec, false, args);
			format_int(writer, &spec, number);
			break;
		}
		case 'o': {
			spec.base = BASE_8;
			number = get_number(&spec, false, args);
			format_int(writer, &spec, number);
			break;
		}
		case 'b': {
			spec.base = BASE_2;
			number = get_number(&spec, false, args);
			format_int(writer, &spec, number);
			break;
		}
		case 'p': {
			spec.base = BASE_PTR;
			number = get_number(&spec, false, args);
			format_int(writer, &spec, number);
		}
		}
	}

	return 0;
}