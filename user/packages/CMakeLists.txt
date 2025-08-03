# Package management system for userspace packages
cmake_minimum_required(VERSION 3.10)

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

# Add subdirectories for individual packages
add_subdirectory(hello_world)
add_subdirectory(libm)
add_subdirectory(lua)
