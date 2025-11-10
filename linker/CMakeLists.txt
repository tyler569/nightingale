add_library(elf STATIC elf-ng.c)

# Apply target properties
nightingale_apply_target_properties(elf)
target_link_libraries(elf PRIVATE autogen_headers)

install(TARGETS elf ARCHIVE DESTINATION ${system_lib_dir})
