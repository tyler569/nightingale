# Lua package - Lua programming language

set(LUA_URL "https://lua.org/ftp/lua-5.4.8.tar.gz")
set(LUA_PACKAGE_DIR "${CMAKE_BINARY_DIR}/packages/lua")

# Download and extract
download_package(lua "${LUA_URL}")

# Find the extracted directory (it will be named lua-5.4.8)
file(GLOB LUA_SRC_DIR "${LUA_PACKAGE_DIR}/lua-*")
list(GET LUA_SRC_DIR 0 LUA_SRC_DIR)

# Apply patches for Nightingale support
apply_patches(lua)

# Define Lua core source files
set(LUA_CORE_SOURCES
    "${LUA_SRC_DIR}/src/lapi.c"
    "${LUA_SRC_DIR}/src/lcode.c"
    "${LUA_SRC_DIR}/src/lctype.c"
    "${LUA_SRC_DIR}/src/ldebug.c"
    "${LUA_SRC_DIR}/src/ldo.c"
    "${LUA_SRC_DIR}/src/ldump.c"
    "${LUA_SRC_DIR}/src/lfunc.c"
    "${LUA_SRC_DIR}/src/lgc.c"
    "${LUA_SRC_DIR}/src/llex.c"
    "${LUA_SRC_DIR}/src/lmem.c"
    "${LUA_SRC_DIR}/src/lobject.c"
    "${LUA_SRC_DIR}/src/lopcodes.c"
    "${LUA_SRC_DIR}/src/lparser.c"
    "${LUA_SRC_DIR}/src/lstate.c"
    "${LUA_SRC_DIR}/src/lstring.c"
    "${LUA_SRC_DIR}/src/ltable.c"
    "${LUA_SRC_DIR}/src/ltm.c"
    "${LUA_SRC_DIR}/src/lundump.c"
    "${LUA_SRC_DIR}/src/lvm.c"
    "${LUA_SRC_DIR}/src/lzio.c"
)

# Define Lua library source files
set(LUA_LIB_SOURCES
    "${LUA_SRC_DIR}/src/lauxlib.c"
    "${LUA_SRC_DIR}/src/lbaselib.c"
    "${LUA_SRC_DIR}/src/lcorolib.c"
    "${LUA_SRC_DIR}/src/ldblib.c"
    "${LUA_SRC_DIR}/src/liolib.c"
    "${LUA_SRC_DIR}/src/lmathlib.c"
    "${LUA_SRC_DIR}/src/loadlib.c"
    "${LUA_SRC_DIR}/src/loslib.c"
    "${LUA_SRC_DIR}/src/lstrlib.c"
    "${LUA_SRC_DIR}/src/ltablib.c"
    "${LUA_SRC_DIR}/src/lutf8lib.c"
    "${LUA_SRC_DIR}/src/linit.c"
)

# Create Lua library
add_library(lua_static STATIC ${LUA_CORE_SOURCES} ${LUA_LIB_SOURCES})
target_include_directories(lua_static PUBLIC "${LUA_SRC_DIR}/src")
target_link_libraries(lua_static c m)
target_compile_definitions(lua_static PRIVATE LUA_USE_POSIX)

# Create Lua interpreter executable
add_executable(lua_pkg "${LUA_SRC_DIR}/src/lua.c")
target_link_libraries(lua_pkg lua_static c m)
target_compile_definitions(lua_pkg PRIVATE LUA_USE_POSIX)

# Create Lua compiler executable
add_executable(luac_pkg "${LUA_SRC_DIR}/src/luac.c")
target_link_libraries(luac_pkg lua_static c m)
target_compile_definitions(luac_pkg PRIVATE LUA_USE_POSIX)

# Install executables and library
install(TARGETS lua_pkg DESTINATION bin RENAME lua)
install(TARGETS luac_pkg DESTINATION bin RENAME luac)
install(TARGETS lua_static DESTINATION lib RENAME lua)
install(FILES "${LUA_SRC_DIR}/src/lua.h" "${LUA_SRC_DIR}/src/luaconf.h" 
              "${LUA_SRC_DIR}/src/lualib.h" "${LUA_SRC_DIR}/src/lauxlib.h"
        DESTINATION include)