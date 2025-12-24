include(${CMAKE_CURRENT_SOURCE_DIR}/LibcSources.cmake)

# Prepend libc directory path to all source files
list(TRANSFORM libc_arch_sources PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
list(TRANSFORM libc_common_sources PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
list(TRANSFORM libc_userland_only_sources PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")

set(libc_all_sources
    ${libc_arch_sources}
    ${libc_common_sources}
    ${libc_userland_only_sources}
)

add_library(c STATIC ${libc_all_sources})

# Apply target properties
nightingale_apply_target_properties(c)
target_link_libraries(c PRIVATE autogen_headers)

install(TARGETS c DESTINATION lib COMPONENT initrd)

set(libc_shared_sources ${libc_all_sources})
list(REMOVE_ITEM libc_shared_sources "${CMAKE_CURRENT_SOURCE_DIR}/x86_64/crt0.S")

add_library(dyc SHARED ${libc_shared_sources})
set_target_properties(dyc PROPERTIES OUTPUT_NAME c)
nightingale_apply_target_properties(dyc)
set_target_properties(dyc PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_link_libraries(dyc PRIVATE autogen_headers)

install(TARGETS dyc DESTINATION lib COMPONENT initrd)
