add_library(elf STATIC elf-ng.c)

# Apply target properties
nightingale_apply_target_properties(elf)
target_link_libraries(elf PRIVATE autogen_headers)

add_executable(ld_ng ldso.c pltstub.S)
set_target_properties(ld_ng PROPERTIES OUTPUT_NAME "ld-ng.so")

# Apply userland target properties
nightingale_userland_target_properties(ld_ng)
target_link_libraries(ld_ng PRIVATE c elf autogen_headers)

install(TARGETS elf DESTINATION lib COMPONENT initrd)
install(TARGETS ld_ng DESTINATION bin COMPONENT initrd)
