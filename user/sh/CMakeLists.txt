list(APPEND SH_SOURCES
    eval.c
    parse.c
    readline.c
    sh.c
    token.c
)

add_executable(sh ${SH_SOURCES})
target_link_libraries(sh crt0)
target_link_libraries(sh c)
install(TARGETS sh RUNTIME DESTINATION bin)
