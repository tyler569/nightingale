add_library(elf STATIC elf-ng.c)

nightingale_apply_target_properties(elf)
target_link_libraries(elf PRIVATE autogen_headers)

install(TARGETS elf DESTINATION lib COMPONENT initrd)

add_library(linker_kernel OBJECT elf-ng.c modld.c)

nightingale_kernel_target_properties(linker_kernel)
target_link_libraries(linker_kernel PRIVATE autogen_headers kernel_headers)
