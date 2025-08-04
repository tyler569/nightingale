cmake_minimum_required(VERSION 3.10)
project(nightingale_sh C)

list(APPEND SH_SOURCES
    eval.c
    parse.c
    readline.c
    sh.c
    token.c
)

add_executable(sh ${SH_SOURCES})
target_link_libraries(sh c)

# Install executable
install(TARGETS sh
    RUNTIME DESTINATION bin
    COMPONENT sh)
