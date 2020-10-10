project(actinidia)
cmake_minimum_required(VERSION 3.4.1)
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
    bass/
)
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

set(CMAKE_CXX_STANDARD 17)
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

string(CONCAT CMAKE_CXX_FLAGS_DEBUG
    "-Wall "            # display all warnings
    "-g "               # with debug information
    "-O0 "               # no optimize
    "-Wno-parentheses"   # disable parentheses warning
)

string(CONCAT CMAKE_CXX_FLAGS_RELEASE
    "-O3"               # enable optimize
)

if (CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release")
endif()
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
