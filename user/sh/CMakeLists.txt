list(APPEND sh_sources
    eval.c
    parse.c
    readline.c
    sh.c
    token.c
)

add_executable(sh ${sh_sources})

# Apply userland target properties
nightingale_userland_target_properties(sh)

target_link_libraries(sh PRIVATE c autogen_headers)

install(TARGETS sh RUNTIME DESTINATION ${system_bin_dir})
