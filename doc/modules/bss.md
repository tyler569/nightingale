# bss.ko - BSS and Data Section Testing Module

## Description

A testing module that demonstrates and validates proper handling of various
data sections in loadable kernel modules. It tests BSS (uninitialized data),
initialized data, and alignment attributes.

## Purpose

- Validates module loader's handling of BSS section
- Tests data section initialization
- Verifies alignment attributes work correctly
- Debugging tool for module loader development

## Functionality

On load, the module:
1. Declares various global variables with different attributes:
   - External declarations
   - Zero-initialized variables (BSS candidates)
   - Non-zero initialized variables (data section)
   - Both scalar integers and arrays
   - Various alignment requirements (64-byte aligned)
2. Prints the addresses of all variables
3. Returns `MODINIT_SUCCESS`

## Variables Tested

### Integers
- `a, b` - External declarations (regular and 64-byte aligned)
- `c, d` - Zero-initialized (BSS candidates)
- `e, f` - Explicitly initialized to 0 (may be BSS or data)
- `g, h` - Initialized to 1 (definitely data section)

### Arrays
- `i, j` - External 100-int arrays
- `k, l` - Zero-initialized arrays
- `m, n` - Arrays explicitly initialized to {0}
- `o, p` - Arrays initialized to {1}

Each pair tests regular vs 64-byte aligned versions.

## Usage

```bash
insmod bss.ko
```

## Expected Output

Prints addresses showing proper placement and alignment:
```
ints:   0xffffffffXXXXXXXX 0xffffffffXXXXXX40 ...
arrays: 0xffffffffXXXXXXXX 0xffffffffXXXXXXC0 ...
```

Note: 64-byte aligned addresses end in 00, 40, 80, or C0.

## Source Code

**Location:** `/kernel/modules/bss.c`

```c
extern int a;
extern int b __attribute__((aligned(64)));

int c;
int d __attribute__((aligned(64)));

int e = 0;
int f __attribute__((aligned(64))) = 0;

int g = 1;
int h __attribute__((aligned(64))) = 1;

// ... similar for arrays i-p ...

int init_mod(struct mod *) {
    printf("ints:   %p %p %p %p %p %p %p %p\n"
           "arrays: %p %p %p %p %p %p %p %p\n",
        (void *)&a, (void *)&b, (void *)&c, (void *)&d,
        (void *)&e, (void *)&f, (void *)&g, (void *)&h,
        (void *)&i, (void *)&j, (void *)&k, (void *)&l,
        (void *)&m, (void *)&n, (void *)&o, (void *)&p);
    return MODINIT_SUCCESS;
}
```

## What It Tests

### BSS Section
- Uninitialized globals (`c`, `d`, `k`, `l`)
- May be placed in BSS to save file size
- Should be zero-initialized at load time

### Data Section
- Initialized globals (`g`, `h`, `o`, `p`)
- Must be in data section with initial values
- Values must survive from file to memory

### Alignment
- Variables with `__attribute__((aligned(64)))`
- Addresses must be multiples of 64
- Tests linker and loader alignment handling

### External Declarations
- `extern` variables (`a`, `b`, `i`, `j`)
- Tests undefined symbol handling
- May cause link errors (expected in this test module)

## Verification

To verify correct operation:
1. Check that aligned variables have aligned addresses
   - Addresses ending in 00, 40, 80, C0 (hex)
2. Verify initialization values if you inspect memory
3. Confirm no crashes during load (proper BSS clearing)

## Notes

- This is primarily a developer/testing tool
- The external declarations will fail to resolve (expected)
- Demonstrates edge cases in ELF loading
- Useful for debugging module loader issues

## See Also

- `/linker/modld.c` - Module loader implementation
- ELF specification - for section types
- `objdump -h bss.ko` - Inspect module sections
