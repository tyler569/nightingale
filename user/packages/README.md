# Nightingale Package Management System

This directory contains the userspace package management system for Nightingale OS. It allows building 3rd party software from local sources or downloaded archives and including them in the tarfs.

## Directory Structure

```
packages/
├── CMakeLists.txt          # Main package system build configuration
├── README.md               # This file
├── hello_world/            # Example local package
│   ├── CMakeLists.txt      # Package build configuration
│   ├── src/                # Local source files
│   │   └── hello.c
│   └── patches/            # Patch files to apply
│       └── add_version.patch
├── libm/                   # Math library package
│   ├── CMakeLists.txt      # Downloads from sortix-libm GitHub
│   └── patches/            # Nightingale-specific patches
└── lua/                    # Lua programming language
    ├── CMakeLists.txt      # Downloads from lua.org
    └── patches/            # Nightingale compatibility patches
        └── nightingale_support.patch
```

## Package Types

### Local Packages (hello_world)
- Source files stored locally in `src/` directory
- Patches applied during build process
- Built from local sources

### Downloaded Packages (libm, lua)
- Sources downloaded from internet during build
- Extracted and cached in build directory
- Patches applied to downloaded sources
- Dependencies handled automatically

## Adding New Packages

1. Create a new directory under `packages/`
2. Create a `CMakeLists.txt` with package configuration
3. For local packages: add source files to `src/` subdirectory
4. For downloaded packages: specify URL in CMakeLists.txt
5. Add any patches to `patches/` subdirectory
6. Update main `packages/CMakeLists.txt` to include new package
7. Update `user/CMakeLists.txt` dependencies if needed

## Build Integration

Packages are built as part of the normal build process and included in the tarfs that gets loaded by the kernel. Built executables go to `/bin`, libraries to `/lib`, and headers to `/include` in the system root.

## Dependencies

Packages can depend on:
- `c` - The Nightingale libc
- `m` - Math library (libm package)
- Other packages built in dependency order

The build system handles downloading, extracting, patching, and building all packages automatically.