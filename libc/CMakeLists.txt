include(${CMAKE_CURRENT_SOURCE_DIR}/LibcSources.cmake)

set(libc_all_sources
    ${libc_arch_sources}
    ${libc_common_sources}
    ${libc_userland_only_sources}
)

add_library(c STATIC ${libc_all_sources})

nightingale_apply_target_properties(c)
target_link_libraries(c PRIVATE autogen_headers)

install(TARGETS c DESTINATION lib COMPONENT initrd)

# ==========================================
# Kernel Libc Component
# ==========================================

add_library(c_kernel OBJECT ${libc_kernel_embedded_sources} ${libc_kernel_arch_sources})

nightingale_kernel_target_properties(c_kernel)
target_link_libraries(c_kernel PRIVATE autogen_headers kernel_headers)
