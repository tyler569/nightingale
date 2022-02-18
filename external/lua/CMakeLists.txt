set(LUA_SOURCES
    src/lauxlib.c
    src/ldblib.c
    src/ldo.c
    src/lgc.c
    src/lapi.c
    src/lmem.c
    src/lcode.c
    src/lstring.c
    src/lctype.c
    src/ldump.c
    src/ldebug.c
    src/lzio.c
    src/ltable.c
    src/lundump.c
    src/lbitlib.c
    src/lmathlib.c
    src/ltablib.c
    src/lobject.c
    src/lparser.c
    src/lutf8lib.c
    src/lvm.c
    src/liolib.c
    src/llex.c
    src/loslib.c
    src/lua.c
    src/ltm.c
    src/lstate.c
    src/lbaselib.c
    src/lopcodes.c
    src/lstrlib.c
    src/loadlib.c
    src/linit.c
    src/lcorolib.c
    src/lfunc.c
)

add_compile_options(
    -Wno-unknown-attributes
    -Wno-string-plus-int
    -Wno-invalid-noreturn
)

include_directories(src)

add_executable(lua ${LUA_SOURCES})
target_link_libraries(lua crt0)
target_link_libraries(lua c)
target_link_libraries(lua m)
install(TARGETS lua RUNTIME DESTINATION bin)
