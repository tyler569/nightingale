# Nightingale C Library Manual Pages

This directory contains manual pages for Nightingale's C library functions (man section 3).
These are the standard library functions available to userland programs.

## Viewing Manual Pages

You can view these pages with any man page viewer or text editor.
If you have `groff` installed:

```bash
groff -man -Tascii printf.3 | less
```

Or with `man` if your MANPATH includes this directory:

```bash
man 3 printf
```

## Library Functions by Category

### Standard I/O (stdio.h)
- **printf**(3), **fprintf**(3), **sprintf**(3), **snprintf**(3) - formatted output
- **puts**(3), **fputs**(3) - output a string
- **fopen**(3), **fclose**(3) - open and close files
- **fread**(3), **fwrite**(3) - read and write data
- **fflush**(3) - flush stream buffers
- **feof**(3), **ferror**(3) - check stream status

### Memory Management (stdlib.h)
- **malloc**(3), **free**(3), **calloc**(3), **realloc**(3) - dynamic memory allocation

### String Manipulation (string.h)
- **strcpy**(3), **strncpy**(3) - copy strings
- **strlen**(3) - get string length
- **strcmp**(3), **strncmp**(3) - compare strings
- **strchr**(3), **strrchr**(3) - locate character in string
- **strstr**(3) - locate substring
- **strpbrk**(3) - search string for any of a set of bytes

### Character Classification (ctype.h)
- **isalnum**(3), **isalpha**(3), **isdigit**(3) - check character type
- **isxdigit**(3), **islower**(3), **isupper**(3) - more character tests
- **isspace**(3), **isblank**(3) - whitespace tests
- **iscntrl**(3), **isprint**(3), **isgraph**(3), **ispunct**(3) - other character tests
- **tolower**(3), **toupper**(3) - character case conversion

### Time Functions (time.h)
- **strftime**(3) - format date and time

### Sorting and Searching (stdlib.h)
- **qsort**(3) - sort an array

### Math and Utility (stdlib.h)
- **abs**(3), **labs**(3), **llabs**(3) - absolute value
- **div**(3), **ldiv**(3), **lldiv**(3) - integer division with quotient and remainder
- **random**(3), **srandom**(3) - pseudo-random number generator
- **atexit**(3) - register exit handler

## Complete Alphabetical List

```
abs(3)        atexit(3)     calloc(3)     ctype(3)
div(3)        fclose(3)     feof(3)       ferror(3)
fflush(3)     fopen(3)      fprintf(3)    fputs(3)
fread(3)      free(3)       fwrite(3)     isalnum(3)
isalpha(3)    isblank(3)    iscntrl(3)    isdigit(3)
isgraph(3)    islower(3)    isprint(3)    ispunct(3)
isspace(3)    isupper(3)    isxdigit(3)   labs(3)
ldiv(3)       llabs(3)      lldiv(3)      malloc(3)
printf(3)     puts(3)       qsort(3)      random(3)
realloc(3)    snprintf(3)   sprintf(3)    srandom(3)
strcmp(3)     strchr(3)     strcpy(3)     strftime(3)
strlen(3)     strncmp(3)    strncpy(3)    strpbrk(3)
strrchr(3)    strstr(3)     string(3)     tolower(3)
toupper(3)
```

## Implementation Notes

The Nightingale C library provides a mostly POSIX-compatible interface for
userland programs, though full POSIX compliance is not a goal.

### Key Implementation Details:

- **Memory allocator**: Uses a free-list based heap with magic numbers for
  corruption detection. Debug builds poison allocated/freed memory.

- **I/O buffering**: Stream I/O (FILE*) is buffered with a default buffer
  size of BUFSIZ. Line buffering is used for terminals.

- **String functions**: All implemented in plain C without SIMD optimizations.
  No locale support.

- **Character classification**: Simple ASCII-only implementation. No locale
  support or wide character handling.

- **Random number generator**: Custom PRNG, not cryptographically secure.

- **Time formatting**: strftime supports most standard format specifiers.
  Timezone is always UTC.

- **Sorting**: qsort uses a basic quicksort implementation with in-place
  partitioning.

## Related Documentation

- `/doc/man2/` - System call manual pages (section 2)
- `/doc/ABOUT.md` - General information about Nightingale
- `/CLAUDE.md` - Development guide
- `/libc/` - C library source code

## Notes

Some library functions may have simplified implementations compared to
full POSIX systems. Limitations are documented in the NOTES section of
each manual page.

For system calls that these library functions wrap, see the corresponding
man 2 pages in `/doc/man2/`.
