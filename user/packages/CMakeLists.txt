# Package management system with dependency resolution
cmake_minimum_required(VERSION 3.10)

# Global tracking variables
set(DECLARED_PACKAGES "" CACHE INTERNAL "List of declared packages")
set(PACKAGE_DEPENDENCIES "" CACHE INTERNAL "Package dependency mappings")
set(PACKAGE_TARGETS "" CACHE INTERNAL "All package build targets")

# Function to declare a package with dependencies
function(declare_package PACKAGE_NAME)
    cmake_parse_arguments(PKG "" "" "DEPENDS" ${ARGN})

    list(APPEND DECLARED_PACKAGES ${PACKAGE_NAME})
    set(DECLARED_PACKAGES ${DECLARED_PACKAGES} CACHE INTERNAL "List of declared packages")

    if(PKG_DEPENDS)
        set(PACKAGE_DEPENDENCIES_${PACKAGE_NAME} ${PKG_DEPENDS} CACHE INTERNAL "Dependencies for ${PACKAGE_NAME}")
    endif()

    message(STATUS "Declared package: ${PACKAGE_NAME} (depends: ${PKG_DEPENDS})")
endfunction()

# Function to include packages in dependency order
function(resolve_and_include_packages)
    set(PROCESSED_PACKAGES "" CACHE INTERNAL "Packages that have been processed")
    set(PROCESSING_PACKAGES "" CACHE INTERNAL "Packages currently being processed")

    # Process packages recursively in dependency order
    foreach(PACKAGE ${DECLARED_PACKAGES})
        include_package_recursive(${PACKAGE})
    endforeach()
endfunction()

# Recursive function to include a package and its dependencies
function(include_package_recursive PACKAGE_NAME)
    # Get current state from cache
    set(PROCESSED ${PROCESSED_PACKAGES})
    set(PROCESSING ${PROCESSING_PACKAGES})

    # Skip if already processed
    if(${PACKAGE_NAME} IN_LIST PROCESSED)
        return()
    endif()

    # Check for circular dependencies
    if(${PACKAGE_NAME} IN_LIST PROCESSING)
        message(FATAL_ERROR "Circular dependency detected: ${PACKAGE_NAME}")
    endif()

    # Mark as being processed
    list(APPEND PROCESSING ${PACKAGE_NAME})
    set(PROCESSING_PACKAGES ${PROCESSING} CACHE INTERNAL "Packages currently being processed")

    # Process dependencies first
    if(DEFINED PACKAGE_DEPENDENCIES_${PACKAGE_NAME})
        foreach(DEP ${PACKAGE_DEPENDENCIES_${PACKAGE_NAME}})
            include_package_recursive(${DEP})
        endforeach()
    endif()

    # Include the package
    include_package_impl(${PACKAGE_NAME})

    # Mark as processed
    set(PROCESSED ${PROCESSED_PACKAGES})
    list(APPEND PROCESSED ${PACKAGE_NAME})
    set(PROCESSED_PACKAGES ${PROCESSED} CACHE INTERNAL "Packages that have been processed")

    # Remove from processing
    set(PROCESSING ${PROCESSING_PACKAGES})
    list(REMOVE_ITEM PROCESSING ${PACKAGE_NAME})
    set(PROCESSING_PACKAGES ${PROCESSING} CACHE INTERNAL "Packages currently being processed")
endfunction()

# Enhanced package inclusion with sysroot installation
function(include_package_impl PACKAGE_NAME)
    set(PACKAGE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${PACKAGE_NAME}")

    if(NOT IS_DIRECTORY "${PACKAGE_DIR}")
        message(FATAL_ERROR "Package directory '${PACKAGE_NAME}' does not exist")
    endif()

    if(NOT EXISTS "${PACKAGE_DIR}/CMakeLists.txt")
        message(FATAL_ERROR "Package '${PACKAGE_NAME}' missing CMakeLists.txt")
    endif()

    message(STATUS "Including package: ${PACKAGE_NAME}")
    add_subdirectory(${PACKAGE_NAME})

    # Create sysroot installation target if package provides one
    if(TARGET ${PACKAGE_NAME}_build)
        add_custom_target(${PACKAGE_NAME}_sysroot_install
            COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ${PACKAGE_NAME}_install_to_sysroot
            DEPENDS ${PACKAGE_NAME}_build
            COMMENT "Installing ${PACKAGE_NAME} to sysroot"
        )

        # Add dependencies to sysroot installation
        if(DEFINED PACKAGE_DEPENDENCIES_${PACKAGE_NAME})
            foreach(DEP ${PACKAGE_DEPENDENCIES_${PACKAGE_NAME}})
                add_dependencies(${PACKAGE_NAME}_sysroot_install ${DEP}_sysroot_install)
            endforeach()
        endif()

        list(APPEND PACKAGE_TARGETS ${PACKAGE_NAME}_sysroot_install)
        set(PACKAGE_TARGETS ${PACKAGE_TARGETS} CACHE INTERNAL "All package sysroot install targets")
    endif()
endfunction()

# Function to download and extract packages
function(download_package PACKAGE_NAME URL)
    set(PACKAGE_DIR "${CMAKE_BINARY_DIR}/packages/${PACKAGE_NAME}")
    set(ARCHIVE_FILE "${CMAKE_BINARY_DIR}/packages/${PACKAGE_NAME}-archive")

    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/packages")

    if(NOT EXISTS "${ARCHIVE_FILE}")
        message(STATUS "Downloading ${PACKAGE_NAME} from ${URL}")
        file(DOWNLOAD "${URL}" "${ARCHIVE_FILE}")
    endif()

    if(NOT EXISTS "${PACKAGE_DIR}")
        message(STATUS "Extracting ${PACKAGE_NAME}")
        file(MAKE_DIRECTORY "${PACKAGE_DIR}")
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E tar xzf "${ARCHIVE_FILE}"
            WORKING_DIRECTORY "${PACKAGE_DIR}"
        )
    endif()
endfunction()

# Function to apply patches to a package
function(apply_patches PACKAGE_NAME)
    set(PACKAGE_DIR "${CMAKE_BINARY_DIR}/packages/${PACKAGE_NAME}")
    set(PATCHES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${PACKAGE_NAME}/patches")

    if(EXISTS "${PATCHES_DIR}")
        file(GLOB PATCH_FILES "${PATCHES_DIR}/*.patch")
        foreach(PATCH_FILE ${PATCH_FILES})
            message(STATUS "Applying patch: ${PATCH_FILE}")
            execute_process(
                COMMAND patch -p1 -i "${PATCH_FILE}"
                WORKING_DIRECTORY "${PACKAGE_DIR}"
                RESULT_VARIABLE PATCH_RESULT
            )
            if(NOT PATCH_RESULT EQUAL 0)
                message(FATAL_ERROR "Failed to apply patch: ${PATCH_FILE}")
            endif()
        endforeach()
    endif()
endfunction()

# Declare packages with their dependencies
declare_package(libm)                    # No dependencies
declare_package(hello_world)             # No dependencies
declare_package(lua DEPENDS libm)        # Depends on libm

# Resolve and include all packages in dependency order
resolve_and_include_packages()

# Create a target for all packages
add_custom_target(all_packages_sysroot)
foreach(TARGET ${PACKAGE_TARGETS})
    add_dependencies(all_packages_sysroot ${TARGET})
endforeach()
