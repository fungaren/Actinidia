include(FetchContent)
set(LUA_VERSION 5.4.3)
set(LUA_URL http://www.lua.org/ftp/lua-${LUA_VERSION}.tar.gz)
set(LUA_MD5 ef63ed2ecfb713646a7fcc583cf5f352)

FetchContent_Declare(lua
    URL ${LUA_URL}
    URL_HASH MD5=${LUA_MD5}
    DOWNLOAD_DIR ${CMAKE_CURRENT_LIST_DIR}/download
)
FetchContent_GetProperties(lua)
if (NOT lua_POPULATED)
    message(STATUS "Downloading lua ${LUA_VERSION} from ${LUA_URL}")
    FetchContent_Populate(lua)
    message(STATUS "Download OK")
endif()
message(STATUS "lua_SOURCE_DIR: ${lua_SOURCE_DIR}")
message(STATUS "lua_BINARY_DIR: ${lua_BINARY_DIR}")

aux_source_directory(${lua_SOURCE_DIR}/src luasrc)
list(REMOVE_ITEM luasrc ${lua_SOURCE_DIR}/src/lua.c ${lua_SOURCE_DIR}/src/luac.c)
add_library(liblua STATIC ${luasrc})
