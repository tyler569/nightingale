#include <ng/common.h>
#include <stdlib.h>
#include <string.h>

char *strcpy(char *dest, const char *src) {
	while (*src != 0) {
		*dest++ = *src++;
	}
	*dest = *src; // copy the \0

	return dest;
}

char *strncpy(char *dest, const char *src, size_t count) {
	int i;
	for (i = 0; i < count && *src != 0; i++) {
		*dest++ = *src++;
	}
	if (i < count) {
		*dest = *src; // copy the \0 if there is room left
	}
	return dest;
}

size_t strlen(const char *s) {
	size_t i = 0;
	while (*s++ != 0) {
		i++;
	}
	return i;
}

int strcmp(const char *a, const char *b) {
	for (size_t i = 0;; i++) {
		if (a[i] != b[i])
			return a[i] - b[i];
		if (a[i] == 0)
			return 0;
	}
}

int strncmp(const char *a, const char *b, size_t count) {
	for (size_t i = 0; i < count; i++) {
		if (a[i] != b[i])
			return a[i] - b[i];
		if (a[i] == 0)
			return 0;
	}
	return a[count - 1] - b[count - 1];
}

char *strchr(const char *s, int c) {
	for (size_t i = 0;; i++) {
		if (s[i] == c)
			return (char *)s + i;
		if (s[i] == 0)
			return 0;
	}
}

char *strrchr(const char *s, int c) {
	size_t len = strlen(s);
	for (ssize_t i = len; i >= 0; i--) {
		if (s[i] == c)
			return (char *)s + i;
	}
	return nullptr;
}

char *strpbrk(const char *s, const char *accept) {
	for (size_t i = 0;; i++) {
		if (s[i] == 0)
			return 0;
		if (strchr(accept, s[i]))
			return (char *)s + i;
	}
}

char *strstr(const char *s, const char *subs) {
	const char *found = nullptr;

	while (1) {
		const char *ss = subs;
		if (*ss == 0) {
			return (char *)found;
		} else if (*s == 0) {
			return nullptr;
		} else if (*s == *ss) {
			s += 1;
			ss += 1;
		} else {
			s += 1;
			ss = subs;
			found = s;
		}
	}
}

void *memchr(const void *pm, int v, size_t count) {
	const unsigned char *mem = pm;
	for (int i = 0; i < count; i++) {
		if (mem[i] == v)
			return (void *)(mem + i);
	}
	return nullptr;
}

int memcmp(const void *pa, const void *pb, size_t count) {
	const unsigned char *a = pa;
	const unsigned char *b = pb;
	for (size_t i = 0; i < count; i++) {
		if (a[i] != b[i])
			return a[i] - b[i];
	}
	return 0;
	/*
	   for (int i = 0; i < count && *a == *b; i++, a++, b++) {
	   }
	   return *b - *a; // test!
	 */
}

void *memset(void *pt, int value, size_t count) {
	unsigned char *dest = pt;
	for (size_t i = 0; i < count; i++) {
		dest[i] = value;
	}
	return dest;
}

#ifdef _NC_WIDE_MEMSET
void *wmemset(void *pt, unsigned short value, size_t count) {
	unsigned short *dest = pt;
	for (size_t i = 0; i < count; i++) {
		dest[i] = value;
	}
	return dest;
}

void *lmemset(void *pt, unsigned int value, size_t count) {
	unsigned *dest = pt;
	for (size_t i = 0; i < count; i++) {
		dest[i] = value;
	}
	return dest;
}

void *qmemset(void *pt, unsigned long value, size_t count) {
	unsigned long *dest = pt;
	for (size_t i = 0; i < count; i++) {
		dest[i] = value;
	}
	return dest;
}
#endif

void *memcpy(void *restrict pt, const void *restrict pc, size_t count) {
	unsigned char *dest = pt;
	const unsigned char *src = pc;

	for (size_t i = 0; i < count; i++) {
		dest[i] = src[i];
	}

	return dest;
}

void *memmove(void *pt, const void *pc, size_t count) {
	unsigned char *dest = pt;
	const unsigned char *src = pc;

	if (dest > src) {
		// move in reverse
		for (ssize_t i = count - 1; i >= 0; i--) {
			dest[i] = src[i];
		}
	} else {
		for (size_t i = 0; i < count; i++) {
			dest[i] = src[i];
		}
	}

	return dest;
}

size_t strspn(const char *str, const char *accept) {
	size_t slen = strlen(str);
	size_t i;
	for (i = 0; i < slen; i++) {
		if (strchr(accept, str[i]) == nullptr)
			break;
	}
	return i;
}

char *strcat(char *dest, const char *src) {
	return strncat(dest, src, 100000); // idk maybe this is a bad idea
}

char *strncat(char *dest, const char *src, size_t count) {
	size_t dest_len = strlen(dest);

	for (size_t i = 0; i < count && src[i] != '\0'; i++) {
		dest[dest_len + i] = src[i];
	}
	return dest;
}

char *strdup(const char *str) {
	size_t str_len = strlen(str);
	char *dest = malloc(str_len + 1);
	memcpy(dest, str, str_len);
	dest[str_len] = 0;
	return dest;
}

char *strndup(const char *str, size_t len) {
	size_t str_len = MIN(strlen(str), len);
	char *dest = malloc(str_len + 1);
	memcpy(dest, str, str_len);
	dest[str_len] = 0;
	return dest;
}

void *memccpy(void *restrict dest, const void *restrict src, int c, size_t n) {
	const char *s = src;
	char *d = dest;

	for (size_t i = 0; i < n; i++) {
		if (s[i] == c)
			return &d[i + 1];
		d[i] = s[i];
	}
	return nullptr;
}
