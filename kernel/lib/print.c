#include <ng/common.h>
#include <ng/panic.h>
#include <stdarg.h>
#include <stdio.h>
#include <stream.h>
#include <string.h>

#define WRITE(data, size) \
	do { \
		int can_write = MIN(size, n - written); \
		F_WRITE(f, data, can_write); \
		written += can_write; \
	} while (0)

enum base : char {
	BASE_10,
	BASE_16,
	BASE_16_CAPS,
	BASE_8,
	BASE_2,
	BASE_PTR,
};

struct format_spec {
	enum base base;
	bool alternate_form;
	bool print_plus;
	bool leave_space;
	bool left_justify;
	char padding_char;
	char int_width;
	short padding_width;
	short precision;
};

struct number {
	uintmax_t value;
	bool negative;
};

__MUST_USE
static int format_pad(FILE *f, struct format_spec *spec, int pad_len, int n) {
	int written = 0;
	for (int i = 0; i < pad_len; i++)
		WRITE(&spec->padding_char, 1);
	return written;
}

__MUST_USE
static struct number get_number(
	struct format_spec *spec, bool signed_, va_list *args) {
	uintmax_t absolute_value;
	bool negative = false;

	if (signed_ || spec->int_width < 0) {
		intmax_t value = 0;
		switch (spec->int_width) {
		case -2 ... 0:
			value = va_arg(*args, int);
			break;
		case 1:
			value = va_arg(*args, long);
			break;
		case 2:
			value = va_arg(*args, long long);
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
			absolute_value = va_arg(*args, unsigned);
			break;
		case 1:
			absolute_value = va_arg(*args, unsigned long);
			break;
		case 2:
			absolute_value = va_arg(*args, unsigned long long);
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
#define NUM_BUF_SIZE (NUM_MAX_DIGITS + 1)

__MUST_USE
static const char *format_layout_int(
	struct format_spec *spec, struct number number, char *buffer) {
	const char *charset = lower_hex_charset;
	uintmax_t divisor;
	uintmax_t acc = number.value;

	if (acc == 0 && spec->base == BASE_PTR) {
		strcpy(buffer, "(nullptr)");
		return buffer;
	} else if (acc == 0) {
		buffer[0] = '0';
		return buffer;
	}

	switch (spec->base) {
	case BASE_10:
		divisor = 10;
		break;
	case BASE_16_CAPS:
		charset = upper_hex_charset;
		[[fallthrough]];
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

	int digit = 0;
	for (; digit < NUM_MAX_DIGITS && acc > 0; digit++) {
		buffer[NUM_MAX_DIGITS - 1 - digit] = charset[acc % divisor];
		acc /= divisor;
	}
	return buffer + (NUM_MAX_DIGITS - digit);
}

__MUST_USE
static int format_number_sign(
	FILE *f, struct format_spec *spec, struct number number, int n) {
	int written = 0;
	if (number.negative) {
		WRITE("-", 1);
	} else if (spec->print_plus) {
		WRITE("+", 1);
	} else if (spec->leave_space) {
		WRITE(" ", 1);
	}
	return written;
}

__MUST_USE
static int format_number_alternate_form(
	FILE *f, struct format_spec *spec, int n) {
	int written = 0;
	if (spec->alternate_form) {
		switch (spec->base) {
		case BASE_2:
			WRITE("0b", 2);
			break;
		case BASE_8:
			WRITE("0", 1);
			break;
		case BASE_10:
			break;
		case BASE_16_CAPS:
			WRITE("0X", 2);
			break;
		case BASE_16:
		case BASE_PTR:
			WRITE("0x", 2);
			break;
		}
	}
	return written;
}

__MUST_USE
static int format_number(
	FILE *f, struct format_spec *spec, struct number number, int n) {
	int written = 0;
	char buf[NUM_BUF_SIZE] = {};
	const char *digits = format_layout_int(spec, number, buf);
	int digits_len = (int)strlen(digits);
	int pad_len = spec->padding_width - digits_len;
	bool null_pointer = number.value == 0 && spec->base == BASE_PTR;

	if (number.negative || spec->print_plus || spec->leave_space) {
		pad_len--;
	}
	if (spec->alternate_form && !null_pointer) {
		switch (spec->base) {
		case BASE_2:
			pad_len -= 2;
			break;
		case BASE_8:
			pad_len--;
			break;
		case BASE_10:
			break;
		case BASE_16_CAPS:
		case BASE_16:
		case BASE_PTR:
			pad_len -= 2;
			break;
		}
	}

	if (spec->padding_width && spec->left_justify) {
		if (!null_pointer) {
			written += format_number_sign(f, spec, number, n - written);
			written += format_number_alternate_form(f, spec, n - written);
		}
		WRITE(digits, digits_len);
		written += format_pad(f, spec, pad_len, n - written);
	} else if (spec->padding_width) {
		if (spec->padding_char == '0' && !null_pointer) {
			written += format_number_sign(f, spec, number, n - written);
			written += format_number_alternate_form(f, spec, n - written);
		}
		written += format_pad(f, spec, pad_len, n - written);
		if (spec->padding_char != '0' && !null_pointer) {
			written += format_number_sign(f, spec, number, n - written);
			written += format_number_alternate_form(f, spec, n - written);
		}
		WRITE(digits, digits_len);
	} else {
		if (!null_pointer) {
			written += format_number_sign(f, spec, number, n - written);
			written += format_number_alternate_form(f, spec, n - written);
		}
		WRITE(digits, digits_len);
	}

	return written;
}

__MUST_USE
static int format_string(
	FILE *f, struct format_spec *spec, const char *s, int n) {
	int written = 0;

	if (s == NULL) {
		s = "(null)";
	}

	int len = strlen(s);
	int pad_len = 0;
	if (spec->precision >= 0) {
		len = MIN(len, spec->precision);
	}
	if (spec->padding_width > len) {
		pad_len = spec->padding_width - len;
	}

	if (spec->padding_width && !spec->left_justify) {
		written += format_pad(f, spec, pad_len, n - written);
		WRITE(s, len);
	} else if (spec->padding_width) {
		WRITE(s, len);
		written += format_pad(f, spec, pad_len, n - written);
	} else {
		WRITE(s, len);
	}
	return written;
}

int vfnprintf(FILE *f, size_t n, const char *format, va_list args_orig) {
	const char *fmt = format;
	int written = 0;
	n -= 1;

	va_list args;
	va_copy(args, args_orig);

	while (*fmt != 0) {
		struct format_spec spec = {
			.base = BASE_10,
			.padding_char = ' ',
			.precision = -1,
		};
		struct number number = {};

		const char *p = strchr(fmt, '%');
		if (!p) {
			WRITE(fmt, strlen(fmt));
			break;
		} else {
			WRITE(fmt, p - fmt);
			fmt = p;
		}

	consume_next:
		switch (*++fmt) {
		case '\0':
		case '%':
			WRITE("%", 1);
			break;
		case '-':
			spec.left_justify = true;
			goto consume_next;
		case '+':
			spec.print_plus = true;
			goto consume_next;
		case ' ':
			spec.leave_space = true;
			goto consume_next;
		case '#':
			spec.alternate_form = true;
			goto consume_next;
		case '0':
			if (spec.precision >= 0) {
				spec.precision *= 10;
			} else if (spec.padding_width) {
				spec.padding_width *= 10;
			} else {
				spec.padding_char = '0';
			}
			goto consume_next;
		case '1' ... '9':
			if (spec.precision >= 0) {
				spec.precision *= 10;
				spec.precision += *fmt - '0';
			} else {
				spec.padding_width *= 10;
				spec.padding_width += *fmt - '0';
			}
			goto consume_next;
		case '*':
			if (spec.precision >= 0) {
				spec.precision = va_arg(args, int);
			} else {
				spec.padding_width = va_arg(args, int);
				if (spec.padding_width < 0) {
					spec.left_justify = true;
					spec.padding_width = -spec.padding_width;
				}
				spec.padding_char = ' '; // this is what gcc seems to do
			}
			goto consume_next;
		case '.':
			spec.precision = 0;
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
			spec.int_width = 2;
			goto consume_next;
		case 'n':
			*va_arg(args, int *) = (int)written;
			break;
		case 'c': {
			char c = va_arg(args, int);
			WRITE(&c, 1);
			break;
		}
		case 's': {
			const char *s = va_arg(args, const char *);
			written += format_string(f, &spec, s, n - written);
			break;
		}
		case 'd':
		case 'i': {
			number = get_number(&spec, true, &args);
			written += format_number(f, &spec, number, n - written);
			break;
		}
		case 'u': {
			number = get_number(&spec, false, &args);
			written += format_number(f, &spec, number, n - written);
			break;
		}
		case 'x': {
			spec.base = BASE_16;
			number = get_number(&spec, false, &args);
			written += format_number(f, &spec, number, n - written);
			break;
		}
		case 'X': {
			spec.base = BASE_16_CAPS;
			number = get_number(&spec, false, &args);
			written += format_number(f, &spec, number, n - written);
			break;
		}
		case 'o': {
			spec.base = BASE_8;
			number = get_number(&spec, false, &args);
			written += format_number(f, &spec, number, n - written);
			break;
		}
		case 'b': {
			spec.base = BASE_2;
			number = get_number(&spec, false, &args);
			written += format_number(f, &spec, number, n - written);
			break;
		}
		case 'p': {
			spec.base = BASE_PTR;
			spec.alternate_form = true;
			spec.int_width = 2;
			number = get_number(&spec, false, &args);
			written += format_number(f, &spec, number, n - written);
			break;
		}
		default:
			printf("invalid format specifier: %c\n", *fmt);
			panic("invalid format specifier");
		}
		fmt++;
	}

	va_end(args);
	F_WRITE(f, "", 1);
	return written;
}

int vsprintf(char *buffer, const char *format, va_list args) {
	struct stream f = sprintf_stream(buffer);
	return vfnprintf(&f, INT_MAX, format, args);
}

int vsnprintf(char *buffer, size_t n, const char *format, va_list args) {
	struct stream f = sprintf_stream(buffer);
	return vfnprintf(&f, n, format, args);
}

int sprintf(char *buffer, const char *format, ...) {
	va_list args;
	va_start(args, format);
	int ret = vsprintf(buffer, format, args);
	va_end(args);
	return ret;
}

int snprintf(char *buffer, size_t n, const char *format, ...) {
	va_list args;
	va_start(args, format);
	int ret = vsnprintf(buffer, n, format, args);
	va_end(args);
	return ret;
}

int fprintf(FILE *f, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	int ret = vfprintf(f, fmt, args);
	va_end(args);
	return ret;
}

int fnprintf(FILE *f, size_t n, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	int ret = vfnprintf(f, n, fmt, args);
	va_end(args);
	return ret;
}

int vfprintf(FILE *f, const char *fmt, va_list args) {
	return vfnprintf(f, INT_MAX, fmt, args);
}

int vprintf(const char *format, va_list args) {
	return vfprintf(w_stdout, format, args);
}

int printf(const char *format, ...) {
	va_list args;
	va_start(args, format);
	int ret = vfprintf(w_stdout, format, args);
	va_end(args);
	return ret;
}

int print(const char *str) { return F_WRITE(w_stdout, str, strlen(str)); }

int puts(const char *str) {
	int ret = F_WRITE(w_stdout, str, strlen(str));
	F_WRITE(w_stdout, "\n", 1);
	return ret;
}