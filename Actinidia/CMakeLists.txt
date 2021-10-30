project(actinidia)
cmake_minimum_required(VERSION 3.4.1)

set(ACTINIDIA_VERSION_MAJOR 1)
set(ACTINIDIA_VERSION_MINOR 0)
set(ACTINIDIA_VERSION_PATCH 0)
message(STATUS "Actinidia version: ${ACTINIDIA_VERSION_MAJOR}."
        "${ACTINIDIA_VERSION_MINOR}.${ACTINIDIA_VERSION_PATCH}")

# find dependencies
find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)
find_package(Lua REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

include_directories(
    ${ZLIB_INCLUDE_DIRS}
    ${PNG_INCLUDE_DIRS}
    ${JPEG_INCLUDE_DIRS}
    ${LUA_INCLUDE_DIRS}
    ${GTK3_INCLUDE_DIRS}
)
link_directories(
    ${CMAKE_INSTALL_PREFIX}/lib/
    bass/
)

# set BASS architecture
if (CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "arm")
    set(BASS "bass-hardfp")
elseif (CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "aarch64")
    set(BASS "bass-aarch64")
elseif (CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "i386")
    set(BASS "bass-x86")
elseif (CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "i686")
    set(BASS "bass-x86")
elseif (CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "x86_64")
    set(BASS "bass-x86_64")
else()
    message(FATAL_ERROR "Unsupported processor: ${CMAKE_HOST_SYSTEM_PROCESSOR}")
endif()
message(STATUS "Using lib${BASS}.so")

# for std filesystem
set(CMAKE_CXX_STANDARD 17)

# add sources
aux_source_directory(../Tools/Common SOURCES)
list(APPEND SOURCES
    "Interface.cpp"
    "Actinidia.cpp"
)
set(target ${PROJECT_NAME})
add_executable(${target} ${SOURCES})
target_link_libraries(
    ${target}
    ${LUA_LIBRARIES}
    ${GTK3_LIBRARIES}
    ${PNG_LIBRARIES}
    ${JPEG_LIBRARIES}
    ${ZLIB_LIBRARIES}
    ${BASS}
    stdc++fs
    pthread
)

add_definitions(
    -D_GTK
)

# set build type and flags
string(CONCAT CMAKE_CXX_FLAGS_DEBUG
    "-Wall "            # display all warnings
    "-g "               # with debug information
    "-O0 "               # no optimize
)
string(CONCAT CMAKE_CXX_FLAGS_RELEASE
    "-O3"               # enable optimize
)
if (CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release")
endif()
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

# default path is /usr/local/
message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")

# set install path
set(ACTINIDIA_INSTALL_PATH "${CMAKE_INSTALL_PREFIX}/share/actinidia")

# install the program
install(TARGETS ${target} RUNTIME
    DESTINATION ${ACTINIDIA_INSTALL_PATH}
)
# create symbolic link to the program
execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink
    ${ACTINIDIA_INSTALL_PATH}/${target} ${target}.symlink)
# copy the symbolic link to /usr/local/bin
install(FILES "${PROJECT_SOURCE_DIR}/actinidia.symlink"
    DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    RENAME ${target}
)

# install the bass library
install(FILES "${PROJECT_SOURCE_DIR}/bass/lib${BASS}.so"
    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)
# preserve runpath
set_property(TARGET ${target} PROPERTY INSTALL_RPATH_USE_LINK_PATH TRUE)

# copy game resource pack if provided
install(FILES "${PROJECT_SOURCE_DIR}/game.res"
    DESTINATION ${ACTINIDIA_INSTALL_PATH}
    PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
    OPTIONAL
)

# copy logo
install(FILES
    "${PROJECT_SOURCE_DIR}/logo.png"
    DESTINATION ${ACTINIDIA_INSTALL_PATH}
    PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)

# create desktop icon
configure_file(
    "${PROJECT_SOURCE_DIR}/actinidia.desktop.in"
    "${PROJECT_BINARY_DIR}/actinidia.desktop"
)
install(FILES
    "${PROJECT_SOURCE_DIR}/actinidia.desktop"
    DESTINATION /usr/share/applications
    PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)

# create package
set(CMAKE_PROJECT_DESCRIPTION "A custom game powered by Actinidia game engine.")
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_DESCRIPTION ${CMAKE_PROJECT_DESCRIPTION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${CMAKE_PROJECT_DESCRIPTION})
set(CPACK_PACKAGE_VERSION_MAJOR ${ACTINIDIA_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${ACTINIDIA_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${ACTINIDIA_VERSION_PATCH})
# set deb package configs
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_FILE_NAME "DEB-DEFAULT")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/mooction/actinidia")
set(CPACK_DEBIAN_PACKAGE_SECTION "games")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "Optional")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "FANG")
set(CPACK_DEBIAN_PACKAGE_DEPENDS
    "libgtk-3-0, zlib1g, libpng16-16, liblua5.3-0, libjpeg62-turbo|libjpeg62"
)
include(CPack)
